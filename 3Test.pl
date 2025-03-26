% Предикат для нахождения минимального элемента в списке
listmin([Min], Min).
listmin([Head|Tail], Min) :-
    listmin(Tail, MinTail),
    (Head < MinTail -> Min = Head; Min = MinTail).

% Предикат для удаления первого вхождения элемента из списка
delete(_, [], []).
delete(X, [X|Tail], Tail) :- !.
delete(X, [Head|Tail], [Head|Rest]) :-
    delete(X, Tail, Rest).

% Предикат для сортировки списка выбором
selection_sort([], []).
selection_sort(List, [Min|Sorted]) :-
    listmin(List, Min),
    delete(Min, List, Rest),
    selection_sort(Rest, Sorted).

% Предикат для чтения списка чисел с клавиатуры
read_list(List) :-
    write('Введите элементы списка через пробел: '),
    read_line_to_string(user_input, Input),
    split_string(Input, " ", "", StringList),
    maplist(string_to_number, StringList, List).

% Преобразование строки в число (с обработкой ошибок)
string_to_number(String, Number) :-
    catch(number_string(Number, String), _, fail).

% Главный предикат программы
main :-
    write('=== Сортировка выбором ==='), nl, nl,
    repeat,
    write('1. Сортировать список'), nl,
    write('2. Выход'), nl,
    write('Выберите действие: '),
    read_line_to_string(user_input, Choice),
    (   Choice = "1" ->
        read_list(List),
        write('Исходный список: '), write(List), nl,
        (   selection_sort(List, Sorted) ->
            write('Отсортированный список: '), write(Sorted), nl, nl
        ;   write('Ошибка сортировки!'), nl, nl
        ),
        fail
    ;   Choice = "2" ->
        write('Выход из программы.'), nl, !
    ;   write('Неверный выбор! Попробуйте снова.'), nl, nl,
        fail
    ).

% Запуск программы
:- initialization(main).
