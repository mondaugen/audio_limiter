import numpy as np
import limiter

l=limiter.limiter(ramp_up=5,ramp_down=3,buffer_size=4)
xs=[
[.5,.75,.5,.75,.5,2,.5,4,.5,0,0,0,0,0,0,0],
[.5,.75,.5,.75,.5,4,.5,3,.5,0,0,0,0,0,0,0],
[.5,.75,.5,.75,.5,4,.5,1.5,.5,0,0,0,0,0,0,0]
]

for x_ in xs:
    x=np.array(x_)
    x_lim=np.zeros_like(x)
    for n in np.arange(0,len(x),l.buffer_size):
        x_lim[n:n+l.buffer_size]=l.tick(x[n:n+l.buffer_size])

    print(x)
    print(x_lim)
    print()
