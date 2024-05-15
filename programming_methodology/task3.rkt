#lang racket

(provide null null? cons pair? car cdr)

(define null 9384750873475) ; jakieś losowe liczby
(define spec 1232134234324)

(define (null? x) (and (number? x) (= x null)))

(define (cons x y)
  (λ (arg) (cond [(= arg 0) x]
                 [(= arg 1) y]
                 [(= arg 2) spec])))

(define (pair? x)
  (and (procedure? x)
       (= (x 2) spec))) ; może się wywalić wywołanie x, ale chyba się nie da inaczej

(define (car x) (x 0))

(define (cdr x) (x 1))