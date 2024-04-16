import vs

def print(signal, intab):
    f = open("outfile.txt", "w")
    f.write("Hello world!")
    f.close()
    #vs.print("hello world") # Available from 2019.1. Print to a pop up message.
