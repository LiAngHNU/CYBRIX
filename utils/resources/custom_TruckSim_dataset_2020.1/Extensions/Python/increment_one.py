import vs  # you need to import vs

# signal is "OPEN".  This is a flag but we won't be using it in this code.
# intab is empty since we are not using in this code.  Using getval instead here.
# think of these just a place holder.

myCounter = 0

def add_one(signal, intab):
    global myCounter 
    sim_time = vs.getval("t")

    if sim_time > 0:
      myCounter = myCounter + 1 
    else:
      myCounter = 0

    vs.var("myCounter").setvalue(myCounter)
    
    return 0.0

    

