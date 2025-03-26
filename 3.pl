% Предикат для нахождения минимального элемента в списке
min_element([Min], Min).
min_element([Head|Tail], Min) :-
    min_element(Tail, MinTail),
    (Head =< MinTail -> Min = Head; Min = MinTail).

% Предикат для удаления первого вхождении элемента из списка
remove_element(X, [X|Tail], Tail).
remove_element(X, [Head|Tail], [Head|Rest]) :-
    remove_element(X, Tail, Rest).

% Предикат для сортировки списка выбором
sort_selection([], []).
sort_selection(List, [Min|Sorted]) :-
    min_element(List, Min),
    remove_element(Min, List, Rest),
    sort_selection(Rest, Sorted).

% Предикат для чтения списка чисел с клавиатуры
read_list(List) :-
    write('Введите элементы списка (через пробел, завершите ввод пустой строкой): '),
    read_line_to_codes(user_input, Codes),
    parse_list(Codes, List).

% Предикат для преобразования строки в список чисел
parse_list([], []).
parse_list(Codes, [Number|Rest]) :-
    % Преобразуем кодировки в строку
    atom_codes(Atom, Codes),
    % Разделим строку на числа
    split_string(Atom, " ", "", StringNumbers),
    maplist(atom_number, StringNumbers, [Number|Rest]).

% Главный предикат программы
main :-
    write('Добро пожаловать в программу сортировки списка выбором!'), nl,
    read_list(List),
    write('Исходный список: '), writeln(List),
    sort_selection(List, Sorted),
    write('Отсортированный список: '), writeln(Sorted).

% Запуск программы
:- main.
