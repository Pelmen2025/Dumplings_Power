% Главный предикат для запуска программы
start :-
    write('Введите натуральное число N: '),
    read_line_to_string(user_input, NStr),
    number_string(N, NStr),
    write('Введите цифру K (0-9): '),
    read_line_to_string(user_input, KStr),
    number_string(K, KStr),
    (   between(0, 9, K) ->
        add_k_to_ends(N, K, Result),
        format('Результат: ~w', [Result]),
        nl
    ;   write('Ошибка: K должна быть цифрой (0-9)'), nl
    ).

% Добавляем цифру K в начало и конец числа N (рекурсивно)
add_k_to_ends(N, K, Result) :-
    number_chars(N, Chars),
    number_chars(K, [KChar]),
    add_k_to_front(Chars, KChar, TempList),  % Рекурсивно добавляем K в начало
    add_k_to_end(TempList, KChar, FinalList), % Рекурсивно добавляем K в конец
    number_chars(Result, FinalList).

% Рекурсивно добавляем K в начало списка (можно было бы просто [KChar | Chars], но для примера рекурсии)
add_k_to_front(List, KChar, [KChar | List]).

% Рекурсивно добавляем K в конец списка
add_k_to_end([], KChar, [KChar]).
add_k_to_end([H | T], KChar, [H | NewT]) :-
    add_k_to_end(T, KChar, NewT).

% Вспомогательный предикат для чтения строки с клавиатуры
read_line_to_string(Stream, String) :-
    read_line_to_codes(Stream, Codes),
    string_codes(String, Codes).

% Запуск программы при загрузке файла
:- initialization(start).
