# SPbAU OS Course
SPbAU-OS-Course (Term 4)

# Файлы
* lock.h / lock.c - реализация spinlock-а.
* thread.h / thread.c - реализация thread-ов. 
* thread_s.S - переключения контекста (идея Маркелова Саши с wrapper-ом. Она прикольная).

# Идея
Всего у меня 2^16 потоков. 
Все потоки хранятся в трех списках:
* free_threads - свободные потоки.
* busy_threads - потоки, которые выполняются.
* terminated_threads - потоки, которые уже завершились.

# Методы
* threads_init - инициализирует потоки. Кладет их в free_threads. Первый поток отвечает за main (выполняется всегда).
* thread_create - достает поток из free_threads и возвращает его.
* thread_start - выделяет память под стэк и кладет поток в busy_threads.
* thread_terminate - кладет поток в terminated_threads.
* thread_join - ждет, пока другой поток не будет TERMINATED.
* thread_destroy - освобождает ресурсы и кладет поток в free_threads.
* thread_yield - переключает поток (кладет текущий поток в конец очереди, берет новый поток из ее начала).
