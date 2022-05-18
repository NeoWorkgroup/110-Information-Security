#!/usr/bin/env python
# -*- coding: utf-8 -*-

# RSA Encryption & Decryption
from lib2to3.refactor import MultiprocessingUnsupported
import random
from re import M
import string
import math
import sys
import os
import base64

primes = [  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,
    53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197,
    199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,
    383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
    577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
    661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761,
    769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977,
    983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
]

if __name__ == '__main__':
    # Generate a random prime number
    def generate_prime(bits):
        while True:
            p = random.randrange(2**(bits-1), 2**bits)
            if is_prime(p) and is_prime_rabin_miller(p):
                #print('*')
                return p
            #else:
            #    print('.', end='', flush=True)

    # Low complexity primality test
    def is_prime(n):
        if n == 2:
            return True
        for i in primes:
            if n % i == 0:
                return False
        return True
    
    # Rabin-Miller primality test
    def is_prime_rabin_miller(n):
        if n == 2:
            return True
        if n % 2 == 0:
            return False
        s = n - 1
        t = 0
        while s % 2 == 0:
            s = s // 2
            t += 1
        for trials in range(25):
            a = random.randrange(2, n - 1)
            v = pow(a, s, n)
            if v != 1:
                i = 0
                while v != (n - 1):
                    if i == t - 1:
                        return False
                    else:
                        i = i + 1
                        v = (v ** 2) % n
        return True
    
    # Multiplicative inverse
    def inverse(a, n):
        t = 0
        newt = 1
        r = n
        newr = a
        while newr != 0:
            quotient = r // newr
            t, newt = newt, t - quotient * newt
            r, newr = newr, r - quotient * newr
        if r > 1:
            return -1
        if t < 0:
            t = t + n
        return t
    
    def intmodpow(a, p, mod): # Square and Multiply Algorithm
        o = 1
        if(p == 0):
            return o
        while(p != 0):
            if(p % 2 == 1):
                o = o * a % mod
            p = p // 2
            if(p != 0):
                a = a * a % mod
        return o

    if(len(sys.argv) < 2):
        print("Usage:")
        print("python B11030233.py -i")
        print("python B11030233.py -e m N e")
        print("python B11030233.py -d m N d")
        print("python B11030233.py -d -CRT m p q d")
        exit(1)

    if sys.argv[1] == '-i':
        p = generate_prime(1024)
        print('p =', p)
        q = generate_prime(1024)
        print('q =', q)
        n = p * q
        print('N =', n)
        phi = (p-1) * (q-1)
        print('phi =', phi)
        e = 65537
        print('e =', e)
        d = inverse(e, phi)
        print('d =', d)

    elif sys.argv[1] == '-e':
        if(len(sys.argv) < 5):
            print("python B11030233.py -e N e")
            exit(1)
        m = int.from_bytes(sys.argv[2].encode('utf-8'), 'big')
        N = int(sys.argv[3])
        e = int(sys.argv[4])

        c = intmodpow(m, e, N)
        print(base64.standard_b64encode(c.to_bytes(math.ceil(c.bit_length()/8), 'big')).decode('ascii'))
    
    elif sys.argv[1] == '-d':
        if(not (len(sys.argv) == 5 or (sys.argv[2] == '-CRT' and len(sys.argv) == 7))):
            print("?Argument Error")
        else:
            if(sys.argv[2] == '-CRT'): # Utilize Chinese Remainder Theorem
                c = int.from_bytes(base64.b64decode(sys.argv[3].encode('utf-8')), 'big')
                p = int(sys.argv[4])
                q = int(sys.argv[5])
                d = int(sys.argv[6])
                m = (c * inverse(d, (p-1)*(q-1))) % (p*q)
                print(m.to_bytes(math.ceil(m.bit_length()/8), 'big'))

            else:
                c = int.from_bytes(base64.b64decode(sys.argv[2].encode('ascii')), 'big')
                N = int(sys.argv[3])
                d = int(sys.argv[4])
                m = intmodpow(c, d, N)
                print(m.to_bytes(math.ceil(m.bit_length()/8), 'big').decode('utf-8'))