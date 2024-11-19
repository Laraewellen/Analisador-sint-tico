program erro_condicional_2;
var
    y: integer;
begin
    y := 7;
    if y < 10 then
        writeln('Y é menor que 10')
    else;  { Erro aqui: o "else" não pode ser seguido apenas de um ponto e vírgula }
end.
