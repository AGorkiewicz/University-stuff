#lang racket

(provide integral)

(define (sum val next a b)
  (if (> a b)
      0
      (+ (val a) (sum val next (next a) b))))

(define (integral f prec)
  (λ(l r)
    (let* ([inc (/ (- r l) prec)]
           [next (λ(x) (+ x inc))]
           [area (λ(x) (* 1.0 (f x) inc))])
      (sum area next l (- r (/ inc 2))))))

(define foo1 (integral (lambda (x) 10) 1000))
(foo1 0 10)

(define foo2 (integral (lambda (x) x) 1000))
(foo2 9 10)
(foo2 0 10)

(define foo3 (integral sin 1000))
(foo3 0 (* pi 2))
(foo3 0 (/ pi 2))

((integral tan 1000) (/ pi -2) (/ pi 2))
