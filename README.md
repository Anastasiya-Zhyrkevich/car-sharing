**#Sharing Car Task**

The solution **input format:**
```
<number of requests>
<request_type> <distance in km> <time in minutes>
...
```

`<request_type>` : 
* `0` if using the car actively, and 
* `1` if in waiting mode

**Partial solution:**

take into consideration packages.

**Restrictions:**

One package should be covered fully by one of the packages. One package can cover more than one time period. 

**Run:**
`g++ main.cpp solver.h solver.cpp -o main --std=c++11`
