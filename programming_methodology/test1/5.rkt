#lang racket

(define (height t)
  (if (null? t) ; ? zakładam, że tu ma być leaf
      0
      (+ 1 (max (height (node-left t))
                (height (node-right t))))))

(define (full? t)
  (if (leaf? t)
      true
      (and (full? (node-left t))
           (full? (node-right t))
           (= (height (node-left t))
              (height (node-right t))))))

(define (full?-2.0 t)
  (define (check t n)
    (if (leaf? t)
        (= n 0)
        (and (check (node-left t)  (- n 1))
             (check (node-right t) (- n 1)))))
  (check t (height t)))

#|

Teza: (full? t) ≡ (full?-2.0 t)
Lemat: (check t n) ≡ true  ⇒  (height t) ≡ n


1. Dla leafa:
(full? null)
≡ true
≡ (check null 0)
≡ (check null (height null))
≡ (full?-2.0 null)


2. Dla dowolnego (node v l r): (zakładając, że teza zachodzi dla l, r)

(full? (node v l r))
≡
(and (full? l)
     (full? r)
     (= (height l)
        (height r)))
≡
(and (full?-2.0 l)
     (full?-2.0 r)
     (= (height l)
        (height r)))
≡
(and (check l (height l))
     (check r (height r))
     (= (height l)
        (height r)))
≡ (możemy dodać implikacje wynikające z pojedynczych warunków)
(and (check l (- (height (node v l r)) 1))
     (check r (- (height (node v l r)) 1))
     (= (height l)
        (height r)))
≡ (z lematu)
(and (check l (- (height (node v l r)) 1))
     (check r (- (height (node v l r)) 1)))
≡
(check (node v l r) (height (node v l r)))
≡
(full?-2.0 (node v l r))

Koniec dowodu (na zasadzie indukcji dla drzew).

|#



