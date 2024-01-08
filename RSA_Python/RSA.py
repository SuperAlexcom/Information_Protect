import random
from Code import Code


class RSA():
    def __init__(self, p: int, q: int, e: int, word: str) -> None:
        self.word = word.upper()
        self.p = p
        self.q = q
        self.e = e

    def get_open_key(self) -> tuple:
        n = self.p * self.q
        open_key = (self.e, n)
        return open_key

    def get_private_key(self) -> tuple:
        f_n = (self.p - 1) * (self.q - 1)
        n = self.p * self.q
        k = 0
        d = ((k * f_n) + 1) / self.e
        if d != int(d):
            while d != int(d):
                k += 1
                d = ((k * f_n) + 1) / self.e
        private_key = (int(d), n)
        return private_key

    def encode(self) -> list:
        word_list = list(self.word)
        encode_word = []
        for i in word_list:
            c_i = Code[i] ** self.get_open_key()[0] % self.get_open_key()[1]
            encode_word.append(c_i)
        return encode_word

    def decode(self) -> None:
        decode_index = []
        for i in self.encode():
            t_i = i ** self.get_private_key()[0] % self.get_private_key()[1]
            decode_index.append(t_i)
        for i in decode_index:
            letters = [k for k, v in Code.items() if v == i]
            print(f"{''.join(letters)} - {i}")