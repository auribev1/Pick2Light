def jsongen(tag_min, tag_max):
    import numpy as np
    tag_num, tag_info = str(np.random.randint(tag_min, tag_max+1)), str(np.random.randint(1, 9999))
    json = {
        "num": tag_num.rjust(3, '0'),
        "info": tag_info.rjust(4, '0'),
    }
    return json
