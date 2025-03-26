% Предикат для объединения числа и цифры в начало
prepend_digit(N, K, Result) :-
    number_chars(N, Chars),
    number_chars(K, [KChar]),
    append([KChar], Chars, NewChars),
    number_chars(Result, NewChars).

% Предикат для объединения числа и цифры в конец
append_digit(N, K, Result) :-
    number_chars(N, Chars),
    number_chars(K, [KChar]),
    append(Chars, [KChar], NewChars),
    number_chars(Result, NewChars).

% Предикат для чтения числа с клавиатуры
read_integer(X) :-
    write('> '),
    read_line_to_codes(user_input, Codes),
    number_codes(X, Codes).

main :-
    writeln('Введите натуральное число (N):'),
    catch(read_integer(N), _, (fail)),
    
    writeln('Введите цифру (k) для добавления:'),
    catch(read_integer(K), _, (fail)),
    
    integer(N), N > 0,
    integer(K), K >= 0, K =< 9, % k должно быть цифрой от 0 до 9
    
    prepend_digit(N, K, NewStart),
    append_digit(NewStart, K, FinalResult),
    
    format('Результат: ~w~n', [FinalResult]),
    
    writeln('Хотите попробовать еще раз? (1 - да, 0 - нет):'),
    catch(read_integer(Answer), _, (writeln('Ошибка ввода!'), fail)),
    (
        Answer = 1 ->
        nl, main;
        writeln('Программа завершена.')
    ).
 
:- main.
