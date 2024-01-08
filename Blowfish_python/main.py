from blowfish import Blowfish
from Tools import read
from Tools import write_file

blowfish = Blowfish()

blowfish.driver('text.txt', 'decoded_text.txt')
