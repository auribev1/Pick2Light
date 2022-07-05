def jsongen(tag_min, tag_max, tags):
    import numpy as np
    json = {}
    for i in range(tags):
        tag_num, tag_info = str(np.random.randint(tag_min, tag_max+1)), str(np.random.randint(1, 9999))
        json[tag_num.rjust(3, '0')] = tag_info.rjust(4, '0')
    return json


def post_info(post):
    post = post[post.objects.map(lambda x: x.value is not None)]
    dicts = {}
    for i in post.objects:
        dicts[i.tag_num] = i.value
        i.value = None
    print(dicts) #Aqui se debe hacer el post al servidor
