#lang racket

(provide eval parse)

;; Składnia wyrażeń arytmetycznych
(struct const     (val)           #:transparent)
(struct binop     (op left right) #:transparent)
(struct unop      (op expr)       #:transparent)
(struct let-expr  (id val expr)   #:transparent)
(struct var-expr  (id)            #:transparent)
(struct if-expr   (eb et ef)      #:transparent)
(struct pair-expr (el er)         #:transparent)
(struct fun       (id expr)       #:transparent)
(struct app       (fun arg)       #:transparent)

(define (op-bin? op)
  (member op '(+ - * / and or = < > apply)))
(define (op-un? op)
  (member op '(fst snd null? pair?)))

(define (expr? e)
  (match e
    [(const v) (or (number? v) (boolean? v) (null? v))]
    [(binop op el er)
     (and (op-bin? op)
          (expr? el)
          (expr? er))]
    [(let-expr x e1 e2)
     (and (symbol? x)  ;; reprezentujemy nazwy zmiennych jako symbole
          (expr? e1)
          (expr? e2))] ;; x jest związany w e2 (ale nie w e1)
    [(var-expr x) (symbol? x)]
    [(if-expr eb et ef)
     (and (expr? eb)
          (expr? et)
          (expr? ef))]
    [(pair-expr el er)
     (and (expr? el)
          (expr? er))]
    [(fun x e)
     (and (symbol? x)
          (expr? e))] ;; x jest związany w e
    [(app e1 e2)
     (and (expr? e1)
          (expr? e2))]
    ;; w predykatach trzeba pamiętać o przypadku domyślnym!
    [_ false]))

;; Obsługa błędów
(struct parse-error (expr))
(struct unbound-var (id))
(struct eval-error  (cause))
(struct type-error  (expected given) #:transparent)

;; Wartości

(struct clo (id body env))

;; W modelu podstawieniowym wartością funkcji jest ona sama
;; W modelu środowiskowym nie jest tak prosto, bo nie podstawiamy
(define (value? e)
  (or (number?  e)
      (boolean? e)
      (clo?     e)
      (and (pair? e)
           (value? (car e))    ; wartość, bo język jest gorliwy
           (value? (cdr e)))))

(define (op->proc op)
  (match op
    ('+ +)
    ('- -)
    ('* *)
    ('/ (lambda (x y) (if (= y 0) (raise (eval-error "Division by zero")) (/ x y))))
    ('= =)
    ('< <)
    ('> >)
    ('and (lambda (x y) (and x y)))
    ('or  (lambda (x y) (or  x y)))
    ('fst (lambda (x) (if (pair? x) (car x) (raise (type-error 'pair x)))))
    ('snd (lambda (x) (if (pair? x) (cdr x) (raise (type-error 'pair x)))))
    ('null? null?)
    ('pair? pair?)
    ('apply apply)))

(struct env (assoc))
(define env-empty (env null))
(define (env-lookup ρ x)
  (let ((res (assoc x (env-assoc ρ))))
    (if res
        (cdr res)
        (raise (unbound-var x)))))
(define (env-add ρ x v)
  (env (cons (cons x v) (env-assoc ρ))))

(define (apply f xs)
  (cond [(null? xs) f]
        [(cons? xs)
         (match f
           [(clo id body env)
            (apply (eval-env body (env-add env id (car xs))) (cdr xs))]
           [_ (raise (type-error 'procedure f))])]
        [else (raise (type-error 'list xs))]))

;; jeśli (expr? e) (env? ρ), to (value? (eval-env e ρ)) (lub błąd)
;; nie doliczymy się do błędu undefined identifier
;; jeśli zmienne wolne e są określone w ρ
(define (eval-env e ρ)
  (match e
    [(const n)    n]
    [(var-expr x) (env-lookup ρ x)]
    [(fun x e)    (clo x e ρ)]
    [(app e1 e2)
     (let* ((v1 (eval-env e1 ρ))
            (v2 (eval-env e2 ρ)))
       (match v1
         [(clo x e ρ) (eval-env e (env-add ρ x v2))]
         [else        (raise (type-error 'procedure v1))]))]
    [(binop op el er)
     (let ((vl (eval-env el ρ))
           (vr (eval-env er ρ)))
       ((op->proc op) vl vr))]
    [(unop op e)
     ((op->proc op) (eval-env e ρ))]
    [(if-expr eb et ef)
     (let ((vb (eval-env eb ρ)))
       (cond
         [(eq? vb true)  (eval-env et ρ)]
         [(eq? vb false) (eval-env ef ρ)]
         [else           (raise (type-error 'boolean vb))]))]
    [(pair-expr el er)
     (cons (eval-env el ρ) (eval-env er ρ))]
    [(let-expr x e1 e2)
     (let ((v (eval-env e1 ρ)))
       (eval-env e2 (env-add ρ x v)))]))

;; zakładamy że (expr? e), wtedy (value? (eval e)) (lub błąd wykonania)
(define (eval e)
  (eval-env e env-empty)) ;; procedury wbudowane lądują w środowisku początkowym

;; (let (x e1) e2) == ((lambda (x) e2) e1)
;; e1 -> v1, potem podstaw v1 za x w e2 i oblicz to wyrażenie

;;;; Parser

(define (tagged-tuple? tag len tup)
  (and (list? tup)
       (pair? tup)
       (eq? tag (car tup))
       (= len (length (cdr tup)))))

;; Symbole zarezerwowane (nie mogą być nazwami zmiennych)
(define (reserved? s)
  (or (op-bin? s)
      (op-un? s)
      (member s '(let if pair lambda null true false quit)))) ; quit zarezerwowane dla repla

;; Dobre nazwy zmiennych: symbole niezarezerwowane
(define (name? s)
  (and (symbol? s)
       (not (reserved? s))))

(define (make-fun ids e)
  (if (null? ids)
      e
      (fun (car ids) (make-fun (cdr ids) e))))

(define (make-app e es)
  (if (null? es)
      e
      (make-app (app e (car es)) (cdr es))))

;; Chcielibyśmy żeby nasz parser przyjmował zacytowane wyrażenia
;; np. '(+ 2 (* 3 5)) -> 2+3*5
;; (let (x e1) e2) <- składnia konkretna dla let-wyrażeń
;; (let (x 3) (+ x x))
;; (pair 3 2) <- składnia konkretna dla par
;; (lambda (x y z) (+ (+ x y) z)) -> (fun x (fun y (fun z ...)))
;; (e1 e2 e3 e4) -> (app (app (app e1 e2) e3) e4)
(define (parse e)
  (cond
    [(number? e)    (const e)]
    [(eq? e 'true)  (const true)]
    [(eq? e 'false) (const false)]
    [(eq? e 'null)  (const null)]
    [(name? e)    (var-expr e)]
    [(and (list? e)
          (= 3 (length e))
          (eq? 'let (car e))
          (list? (cadr e))
          (= 2 (length (cadr e)))
          (name? (car (cadr e))))
     (let-expr (caadr e) (parse (cadr (cadr e))) (parse (caddr e)))]
    [(and (list? e)
          (= 4 (length e))
          (eq? 'if (first e)))
     (if-expr (parse (second e)) (parse (third e)) (parse (fourth e)))]
    [(and (list? e)
          (= 3 (length e))
          (eq? 'pair (first e)))
     (pair-expr (parse (second e)) (parse (third e)))]
    [(and (list? e)
          (< 0 (length e))
          (eq? 'list (first e)))
     (foldr (λ (x xs) (pair-expr (parse x) xs)) (const null) (cdr e))]
    [(and (list? e)
          (= 2 (length e))
          (op-un? (first e)))
     (unop (first e) (parse (second e)))]
    [(and (list? e)
          (= 3 (length e))
          (eq? 'lambda (first e))
          (list? (second e))
          (pair? (second e))
          (andmap name? (second e)))
     (make-fun (second e) (parse (third e)))]
    [(and (list? e)
          (= 3 (length e))
          (op-bin? (car e)))
     (binop (car e) (parse (cadr e)) (parse (caddr e)))]
    [(and (list? e)
          (> (length e) 1))
     (make-app (parse (car e)) (map parse (cdr e)))]
    [else (raise (parse-error e))]))

;;;; REPL

;; Prosta pętla czytająca, parsująca i wykonująca programy,
;; i wyświetlająca ich wartości (lub komunikaty o błędach).
;; Polecenie 'quit' kończy działanie ewaluatora.
(define (repl)
  (define (disp-pe pe)
    (display "Parse error: ")
    (displayln (parse-error-expr pe))
    (go))
  (define (disp-te te)
    (display "Type error! Expected: ")
    (display (type-error-expected te))
    (display ",
but given: ")
    (displayln (type-error-given te))
    (go))
  (define (disp-uv uv)
    (display "Unbound variable: ")
    (displayln (unbound-var-id uv))
    (go))
  (define (disp-ee ee)
    (display "Runtime error: ")
    (displayln (eval-error-cause ee))
    (go))
  (define (go)
    (with-handlers
        ([parse-error? disp-pe]
         [type-error?  disp-te]
         [unbound-var? disp-uv]
         [eval-error?  disp-ee])
      (display "> ")
      (let ((text (read)))
        (unless (eq? text 'quit)
          (let* ((prog (parse text))
                 (val  (eval prog)))
            (displayln val)
            (go))))))
  (go))