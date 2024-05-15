#lang racket

(define (kwadraty-liczb-parzystych xs)
  (define (square x) (* x x))
  (define (even x) (if (= (modulo x 2) 0) #t #f))
  (map square (filter even xs)))

(define (aplikuj-procedury fs x)
  (define (merge now res) (now res))
  (foldr merge x fs))

(define (wstaw-pomiedzy x xs)
  (define (merge now res) (cons now (cons x res)))
  (cons x (foldr merge null xs)))

(define (grupuj xs) ; zakładam, że nie mam niczego sortować.
  (define (merge now res)
    (cond [(null? res) (list (list now))]
          [(= (car (car res)) now) (cons (cons now (car res)) (cdr res))]
          [else (cons (list now) res)]))
  (foldr merge null xs))
