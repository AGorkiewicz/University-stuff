#|

Pokażmy lemat - dla każdego xs, acc:
  (iter xs acc) ≡ (+ acc (iter xs 0))

1. Dla nulla:
  (iter null acc) ≡ acc ≡ (+ acc 0) ≡ (+ acc (iter null 0))

2. Dla (cons x xs): (zakładając, że warunek zachodzi dla xs)
  (iter (cons x xs) acc)
≡ (iter xs (+ (f x) acc))
≡ (+ (+ (f x) acc) (iter xs 0))
≡ (+ (f x) acc (iter xs 0))
≡ (+ acc (+ (f x) (iter xs 0)))
≡ (+ acc (iter xs (f x)))
≡ (+ acc (iter (cons xs x) 0))

Koniec dowodu lematu (na zasadzie indukcji na listach).


Dowód właściwego twierdzenia:

1. Dla nulla:
  (map-sum f null)
≡ (iter null 0)
≡ 0
≡ (sum null)
≡ (sum (map f null))

2. Dla (cons x xs): (zakładając, że główne założenie zachodzi dla xs) (oraz lemat)
  (map-sum f (cons x xs)
≡ (iter (cons x xs) 0)
≡ (iter xs (f x))
≡ (+ (f x) (iter xs 0))
≡ (+ (f x) (map-sum f xs))
≡ (+ (f x) (sum (map f xs)))
≡ (sum (cons (f x) (map f xs)))
≡ (sum (map (f (cons x xs)))

Koniec dowodu.

|#