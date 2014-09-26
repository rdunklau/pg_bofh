pg_bofh
=======

pg_bofh is a simple extension providing a planner hook example on how to analyze
incoming queries to forbid some of them.

How to use
==========

Compile and install it:

```
make && sudo make install
```

Add it to the shared_preload_libraries parameter in postgresql.conf, and restart
your server:

```
shared_preload_libraries='pg_bofh'
```

From now on, any query with no "WHERE" clause will raise an error instead of
being executed.
