#lang racket

(provide (struct-out const) (struct-out binop) (struct-out var-expr) (struct-out let-expr) (struct-out var-dead) find-dead-vars)

; --------- ;
; Wyrazenia ;
; --------- ;

(struct const    (val)      #:transparent)
(struct binop    (op l r)   #:transparent)
(struct var-expr (id)       #:transparent)
(struct var-dead (id)       #:transparent)
(struct let-expr (id e1 e2) #:transparent)

(define (expr? e)
  (match e
    [(const n) (number? n)]
    [(binop op l r) (and (symbol? op) (expr? l) (expr? r))]
    [(var-expr x) (symbol? x)]
    [(var-dead x) (symbol? x)]
    [(let-expr x e1 e2) (and (symbol? x) (expr? e1) (expr? e2))]
    [_ false]))

(define (parse q)
  (cond
    [(number? q) (const q)]
    [(symbol? q) (var-expr q)]
    [(and (list? q) (eq? (length q) 3) (eq? (first q) 'let))
     (let-expr (first (second q))
               (parse (second (second q)))
               (parse (third q)))]
    [(and (list? q) (eq? (length q) 3) (symbol? (first q)))
     (binop (first q)
            (parse (second q))
            (parse (third q)))]))

; ---------- ;
; Srodowiska ;
; ---------- ;

(struct environ (xs))

(define env-empty (environ null))
(define (env-add x v env)
  (environ (cons (cons x v) (environ-xs env))))
(define (env-lookup x env)
  (define (assoc-lookup xs)
    (cond [(null? xs) #f]
          [(eq? x (car (car xs))) (cdr (car xs))]
          [else (assoc-lookup (cdr xs))]))
  (assoc-lookup (environ-xs env)))

; ---------------------------------- ;
; Wyszukaj ostatnie uzycie zmiennych ;
; ---------------------------------- ;

(define (find-dead-vars e)
  (define (dfs e env)
    (match e
      [(const n) (cons (const n) env)]
      [(binop op l r)
       (let* ([right-res (dfs r env)]
              [env (cdr right-res)]
              [left-res (dfs l env)]
              [env (cdr left-res)])
         (cons (binop op (car left-res) (car right-res)) env))]
      [(var-expr x)
       (let* ([dead (env-lookup x env)])
         (if dead
             (cons (var-expr x) env)
             (cons (var-dead x) (env-add x true env))))]
      [(let-expr x e1 e2)
       (let* ([now (env-lookup x env)]
              [env (env-add x false env)]
              [right-res (dfs e2 env)]
              [env (cdr right-res)]
              [env (env-add x now env)]
              [left-res (dfs e1 env)]
              [env (cdr left-res)])
         (cons (let-expr x (car left-res) (car right-res)) env))]
      [_ (error "???")]))
  (car (dfs e env-empty)))

#|
(find-dead-vars (let-expr 'x (const 3)
                          (binop '+ (var-expr 'x)
                                 (let-expr 'x (const 5)
                                           (binop '+ (var-expr 'x)
                                                  (var-expr 'x))))))
|#