program erro2;
var
    x: integer;
    y: real;
begin
    x := 10;
    y := x + "texto";  // Erro de tipo: operação inválida entre um número e uma string
    x := x + 1;
end.
