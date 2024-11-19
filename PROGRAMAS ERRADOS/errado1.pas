program erro1;
var
    b: integer;
begin
    b := 8;
    if b > 5  { Erro aqui: falta o "then" após a condição do "if" }
        writeln('B é maior que 5')
    else
        writeln('Else executado');
end.
