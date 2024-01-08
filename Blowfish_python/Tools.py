from keys import S


def F(x):
    temp = S[0][x >> 24]
    temp = (temp + S[1][x >> 16 & 0xff]) % (2 ** 32)
    temp = temp ^ S[2][x >> 8 & 0xff]
    temp = (temp + S[3][x & 0xff]) % (2 ** 32)
    return temp


def swap(a, b):
    temp = a
    a = b
    b = temp
    return a, b


def read(filename):
    file_data = []
    with open(filename, 'rb') as file:
        some_char = file.read(4)
        while some_char != b"":
            file_data.append(some_char)
            some_char = file.read(4)
        if len(file_data[-1]) != 4:
            file_data[-1] = file_data[-1] + b" " * (4 - len(file_data[-1]))
        if len(file_data) % 2 != 0:
            file_data.append(b"    ")
        int_data = []
        for word in file_data:
            int_data.append(int.from_bytes(word, 'big'))

    return int_data


def write_file(filename, int_data):
    file_data = []
    for word in int_data:
        file_data.append(word.to_bytes(4, 'big'))

    with open(filename, 'wb') as file:
        file.write(b''.join(file_data))

