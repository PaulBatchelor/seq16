(
_seq "./seq16.so" fl

_clk var

0 120 4 clock _clk set

_clk get "pat.spa" _gate _seq fe

_clk get 0 _gate tget * 0.001 0.01 0.001 tenvx 250 0.5 sine *
_clk get 1 _gate tget * 0.001 0.01 0.001 tenvx 500 0.5 sine *
_clk get 2 _gate tget * 0.001 0.01 0.001 tenvx 1000 0.5 sine *
_clk get 3 _gate tget * 0.001 0.01 0.001 tenvx 2000 0.5 sine *
+ + +

dup
_seq fc
)
