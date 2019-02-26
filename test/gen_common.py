
def complete_array(a,s,form="%.18f,\n"):
    for x in a:
        s+=form%(x,)
    s+="""};
    """
    return s

