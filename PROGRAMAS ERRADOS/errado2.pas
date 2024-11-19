program erro2;
var
    c: integer;
begin
    c := 6;
    if  { Erro aqui: falta a condição após o "if" }
    else
        writeln('Else executado');
end.
