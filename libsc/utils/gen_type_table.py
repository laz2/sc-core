#!/usr/bin/env python

TGF_SYNTACTIC  = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06]
TGF_CONSTANCY  = [0x08, 0x10, 0x18]
TGF_FUZZYNESS  = [0x00, 0x20, 0x40]
TGF_PERMANENCY = [0x00, 0x80]
TGF_STRUCTURE  = [0x00, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xE0]

SC_SYNTACTIC  = [0x01, 0x02, 0x04, 0x08, 0x10, 0x20]
SC_CONSTANCY  = [0x40, 0x80, 0x100]
SC_FUZZYNESS  = [0x200, 0x400, 0x800]
SC_PERMANENCY = [0x1000, 0x2000]
SC_STRUCTURE  = [0x00, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000]

mattr = {}
mattr2 = {}

for i in range(len(SC_CONSTANCY)):
    for k in range(len(SC_SYNTACTIC)):
        ind = TGF_CONSTANCY[i]|TGF_SYNTACTIC[k]
        val = SC_CONSTANCY[i]|SC_SYNTACTIC[k]

        if k == 1: # If syntactic type is sc-node
            for j in range(len(SC_STRUCTURE)):
                mattr[ind|TGF_STRUCTURE[j]] = val|SC_STRUCTURE[j]
        elif k == 4: # If syntactic type is accessory sc-arc
            for j in range(len(SC_FUZZYNESS)):
                for l in range(len(SC_PERMANENCY)):
                    mattr[ind|TGF_FUZZYNESS[j]|TGF_PERMANENCY[l]] = val|SC_FUZZYNESS[j]|SC_PERMANENCY[l]
        else:
            mattr[ind] = val

TGF_MAX_IDX = 0
SC_MAX_IDX  = 0

for k, v in mattr.items():
    TGF_MAX_IDX = max(k, TGF_MAX_IDX)
    SC_MAX_IDX  = max(v, SC_MAX_IDX)
    mattr2[v] = k

def get_line_start(line_len):
    if line_len == 0:
        return "\t"
    else:
        return " "

def print_table(name, max_idx, mattr):
    s = "int %s[%d] = {\n" % (name, max_idx + 1)
    cur_len = 0
    for i in range(max_idx):
        line = get_line_start(cur_len)

        line += str(mattr.get(i, -1)) + ","
        cur_len += len(line)

        s = s + line

        if cur_len >= 80:
            s += "\n"
            cur_len = 0

    s += get_line_start(cur_len) + str(mattr.get(max_idx, -1))
    s += "\n};"
    print s

print_table("tgf2sc_type_table", TGF_MAX_IDX, mattr)
print_table("sc2tgf_type_table", SC_MAX_IDX, mattr2)
