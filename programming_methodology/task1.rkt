#lang racket

(define (abs x)
  (cond [(> x 0) x]
        [(= x 0) x]
        [else (- x)]))

(define (average a b)
  (/ (+ a b) 2))

(define (square a) (* a a))
(define (cube a) (* (square a) a))

(define (cube-root x)
  
  (define (improve y)
    (/ (+ (/ x (square y))
          (* 2 y))
       3))

  (define (cbrt-iter guess)
    (if (good-enough? guess)
        guess
        (cbrt-iter (improve guess))))

  (define (good-enough? guess)
    (< (abs (- (cube guess)
               x))
       0.0000001))
  
  (if (>= x 0)
      (cbrt-iter 1.0)
      (- (cube-root (- x)))))

(provide cube-root)