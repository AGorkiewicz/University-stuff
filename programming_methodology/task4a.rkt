#lang racket

(provide caesar)

(define (order_of_val tab val)
  (if (eq? (car tab) val)
      0
      (+ 1 (order_of_val (cdr tab) val))))

(define (find_by_order tab order)
  (if (= order 0)
      (car tab)
      (find_by_order (cdr tab) (- order 1))))

(define (caesar alphabet key)
  (define len (length alphabet))
  (define (norm pos) (modulo pos len))
  (define (shift +/-)
    (λ (letter)
      (find_by_order alphabet (norm (+/- (order_of_val alphabet letter) key)))))
  (cons (shift +) (shift -)))

; (define a-z (string->list "abcdefghijklmnopqrstuvwxyz"))
; (define ring(caesar a-z 1))
; (list->string (map (car ring) (string->list "abcxyz")))
; (list->string (map (cdr ring) (string->list "bcdyza")))
; (map (car (caesar (list 0 1 2 'a 'b 'c) 2)) (list 0 2 'c 'c 1 2 'b 'a))