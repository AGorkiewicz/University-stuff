#lang racket

(provide crack-caesar)

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

;; ==============
;; Prosty słownik
;; ==============

(define dict-empty null)

(define (dict-insert k v d)
  (if (null? d)
      (list (cons k v))
      (if (eq? k (caar d))
          (cons (cons k v)
                (cdr d))
          (cons (car d)
                (dict-insert k v (cdr d))))))

(define (dict-lookup k d)
  (if (null? d)
      'not-found
      (if (eq? k (caar d))
          (cdar d)
          (dict-lookup k (cdr d)))))

(define (to-assoc-list d) d)

;; ===========
;; Rozwiązanie
;; ===========
   
(define (crack-caesar alphabet c text)

  (define (dict-increase k d)
    (let ([res (dict-lookup k d)])
      (dict-insert k (if (number? res) (+ res 1) 1) d)))

  (define (max-pair a b)
    (if (> (cdr a) (cdr b)) a b))
  
  (define (most_common tab)
    (car (foldr max-pair (cons -1 -1) tab)))

  (define (apply f tab)
    (foldr (λ (val res) (cons (f val) res)) null tab))
    
  (let* ([len (length alphabet)]
         [dict (foldr dict-increase dict-empty text)]
         [x (most_common (to-assoc-list dict))]
         [pos1 (order_of_val alphabet c)]
         [pos2 (order_of_val alphabet x)]
         [diff (modulo (- pos1 pos2) len)]
         [shifting-gear (car (caesar alphabet diff))]
         [result (apply shifting-gear text)])
     result))

;(define az (string->list "abcdefghijklmnopqrstuvwxyz"))
;(define text (string->list "abcxddddxd"))
;(define c #\e)
;(crack-caesar az c text)

