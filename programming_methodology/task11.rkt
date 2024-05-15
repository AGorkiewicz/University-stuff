#lang racket

(require racket/contract)

(provide (contract-out
          [with-labels with-labels/c]
          [foldr-map foldr-map/c]
          [pair-from pair-from/c]))
(provide with-labels/c foldr-map/c pair-from/c)

; ---------------------------------------------------

(define with-labels/c
  (parametric->/c [a b] (-> (-> a b)
                            (listof a)
                            (listof (cons/c b (cons/c a null))))))
  
(define (with-labels f xs)
  (foldr (λ(x y) (cons (list (f x) x) y)) null xs))

; ---------------------------------------------------

(define foldr-map/c
  (parametric->/c [a b c] (-> (-> a c (cons/c b c))
                              c
                              (listof a)
                              (cons/c (listof b) c))))

(define (foldr-map f acc xs)
  (define (gao x y)
  (let ((res (f x (cdr y))))
    (cons (cons (car res) (car y)) (cdr res))))
  (foldr gao (cons null acc) xs))

; ---------------------------------------------------

(define pair-from/c
  (parametric->/c [a b c] (-> (-> a b)
                              (-> a c)
                              (-> a (cons/c b c)))))

(define (pair-from f g)
  (λ(x) (cons (f x) (g x))))

; owoc ciężkiej pracy: Adama Górkiewicza, Marcina Martowicza, Tytusa Grodzickiego
