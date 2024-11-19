program erro_condicional3;
var
    x: integer;
begin
    x := 10;
    if x > 5 then
        writeln('X é maior que 5')
    else;  { Erro aqui: o "else" está vazio ou mal estruturado }
end.
