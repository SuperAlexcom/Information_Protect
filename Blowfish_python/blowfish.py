from keys import P, S, K
from Tools import F, swap, read, write_file


class Blowfish:

    def __init__(self):
        self.p = []

    def driver(self, data: str, output_data: str) -> None:
        self.expand()

        data = read(data)
        print(f"4 byte source chunks: {[i.to_bytes(4, 'big') for i in data]}\n")

        enc = [0 for i in data]
        for i in range(0, len(data), 2):
            enc[i], enc[i + 1] = self.encrypt(data[i], data[i + 1])

        print(f"4 byte encoded chunks: {[i.to_bytes(4, 'big') for i in enc]} \n")
        dec = [0 for i in enc]
        for i in range(0, len(enc), 2):
            dec[i], dec[i + 1] = self.decrypt(enc[i], enc[i + 1])
        print(f"Decoded text: {[b''.join([i.to_bytes(4, 'big') for i in dec]).decode()]}\n")
        write_file(output_data, dec)

    def expand(self) -> None:
        for i in range(18):
            self.p.append(K[i % len(K)] ^ P[i])
        ki = 0
        li = 0
        for i in range(0, 18, 2):
            ki, li = self.encrypt(ki, li)
            self.p[i] = ki
            self.p[i + 1] = li
        for i in range(4):
            for j in range(0, 256, 2):
                ki, li = self.encrypt(ki, li)
                S[i][j] = ki
                S[i][j + 1] = li

    def encrypt(self, left:int, right:int) -> tuple[int, int]:
        for i in range(0, 16):
            left ^= self.p[i]
            right ^= F(left)
            left, right = swap(left, right)
        left, right = swap(left, right)
        left ^= self.p[17]
        right ^= self.p[16]
        return left, right

    def decrypt(self, left:int, right:int) -> tuple[int, int]:
        for i in range(17, 1, -1):
            left ^= self.p[i]
            right ^= F(left)
            left, right = swap(left, right)
        left, right = swap(left, right)
        left ^= self.p[0]
        right ^= self.p[1]
        return left, right
