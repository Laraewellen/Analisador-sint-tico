program exemplo2;
var
    a, b: integer;
    c: real;
begin
    a := 5;
    b := 10;
    c := a + b;  // Soma de inteiros, atribuição para real
    if c > 10 then
        b := b + 1;
    while b < 15 do
        b := b + 1;
end.
