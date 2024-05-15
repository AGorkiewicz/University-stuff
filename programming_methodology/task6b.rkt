#lang racket

(provide (struct-out const) (struct-out binop) rpn->arith)

;; -------------------------------
;; Wyrazenia w odwr. not. polskiej
;; -------------------------------

(define (rpn-expr? e)
  (and (list? e)
       (pair? e)
       (andmap (lambda (x) (or (number? x) (member x '(+ - * /))))
               e)))

;; ----------------------
;; Wyrazenia arytmetyczne
;; ----------------------

(struct const (val)    #:transparent)
(struct binop (op l r) #:transparent)

(define (arith-expr? e)
  (match e
    [(const n) (number? n)]
    [(binop op l r)
     (and (symbol? op) (arith-expr? l) (arith-expr? r))]
    [_ false]))

;; ----------
;; Kompilacja
;; ----------

(define (rpn->arith e)
  (define (aux e s)
    (cond
      [(and (null? e)
            (cons? s)
            (null? (cdr s)))
       (car s)]
      [(number? (car e))
       (aux (cdr e) (cons (const (car e)) s))]
      [(or (null? e) ; oczekuję, że (car e) to operator lub null (bo spełnia predykat rpn)
           (null? s)
           (null? (cdr s)))
       (error "Wrong number of arguments on the stack")]
      [else ; oczekuję, że (car e) to operator i na stacku są min. 2 expry
       (let* ((x (car  s))
              (y (cadr s))
              (s (cddr s))
              (s (cons (binop (car e) y x) s)))
         (aux (cdr e) s))]))
  (if (rpn-expr? e)
      (aux e null)
      (error "Input is not an RPN expression")))

; (define r '(5 3 * 2 +))