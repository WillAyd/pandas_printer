# What is this?

On it's own merits this project is not very useful, and likely never will be as a standalone project. The sole pupose of this project is to provide a separate repository to start experimentation with what could form into a pandas C API upstream. Currently there is no low level way to generically access the data stored in a pandas DataFrame - the functions shown here in their intial state show a way to do this and print the contents.  Currently this *works only with integers*, but incrementally more types will be added as we simultaneously work on an actual C API.

This could be initially upstreamed in pandas to simplify a lot of the JSON code that uses a lot of state management to iterate a DataFrame across both axes, though there are many more possible applications.

# How to build

`python setup.py build_ext --inplace` from the root directory

# How to use

```python
import numpy as np

import libprinter

arrs = [np.array([1, 2]), np.array([5, 6]), np.array([3, 4])]
libprinter.print_arrs(arrs)
```

This code should yield the following in stdout

```sh
Here is the data printed by row
1 2
5 6
3 4
Here is the data printed by column
1 5 3
2 6 4
```

In a near iteration we would replace `arrs` with an actual DataFrame, and internally use the new df._mgr.column_arrays property instead of manually constructing a list of arrays
