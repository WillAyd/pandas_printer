# What is this?

On it's own merits this project is not very useful, and likely never will be as a standalone project. The sole pupose of this project is to provide a separate repository to start experimentation with what could form into a pandas C API upstream. Currently there is no low level way to generically access the data stored in a pandas DataFrame - the functions shown here in their intial state show a way to do this and print the contents.  Currently this *works only with integers*, but incrementally more types will be added as we simultaneously work on an actual C API.

This could be initially upstreamed in pandas to simplify a lot of the JSON code that uses a lot of state management to iterate a DataFrame across both axes, though there are many more possible applications.

# How to build

`python setup.py build_ext --inplace` from the root directory

# How to use

```python
import pandas as pd

import libprinter

df = pd.DataFrame([[1, 3, 5], [2, 4, 6]])
libprinter.print_frame(df)
```

This code should yield the following in stdout

```sh
Here is the data printed by row
1 3 5
2 4 6
Here is the data printed by column
1 2
3 4
5 6
```
