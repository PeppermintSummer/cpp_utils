import numpy as np


# @brief: load_tensor from c/c++
# @author: peppermintsummer

def load_tensor(file):
    with open(file,'rb') as f:
        data = f.read()
    magic_number, ndims, dtype = np.frombuffer(data,np.uint32,count=3, offset=0)
    assert magic_number == 0xFCCFE2E2, f'{file} is not your tensor'
    dims = np.frombuffer(data, dtype=np.uint32, count=ndims, offset=3 * 4) # count:store byte count，
    if dtype == 0:
        np_dtype = np.float32
    elif dtype == 1:
        np_dtype = np.int32
    else:
        assert False, 'encount unsupport dtype'
    return np.frombuffer(data, np_dtype, offset=(ndims + 3) * 4).reshape(*dims)


data = load_tensor('tensor.bin')
print(data)
print(data.dtype)