/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

/**
 * @defgroup libsc libsc - sc-memory modeling library
 * @{
 */

/**
 * @file libsc.h
 * @brief Интерфейсный файл библиотеки libsc.
 */

#ifndef __LIBSC_H__
#define __LIBSC_H__

#ifdef _MSC_VER
	#pragma warning (disable:4786)
	#pragma warning (disable:4996)
#endif

#include <assert.h>

#include "config.h"

#include "sc_addr.h"
#include "sc_types.h"
#include "sc_content.h"
#include "sc_constraint.h"
#include "sc_event.h"
#include "sc_iterator.h"
#include "sc_segment.h"
#include "segtable.h"
#include "std_constraints.h"

#include <libsys/concurrent.h>

/// Iterator over objects (sc-segments and sc-directories) in sc-directory.
class LIBSC_API sc_dir_iterator
{
public:

	/**
	 * @return имя объекта, на который указывает итератор.
	 */
	virtual const sc_string value() = 0;

	/**
	 * @brief Установка итератора на следующий объект.
	 * @return false, если не существует следующего объекта.
	 * @return true - в противном случае.
	 */
	virtual bool next() = 0;

	/**
	 * @return true, если не существует объектов для итерации.
	 * @return false - в противном случае.
	 */
	virtual bool is_over() = 0;

	virtual ~sc_dir_iterator() {}
};

/// Typedef std::auto_ptr + sc_dir_iterator
typedef std::auto_ptr<sc_dir_iterator> auto_sc_dir_iterator_ptr;

/**
 * @brief Базовый класс доступа к sc-памяти.
 */
class LIBSC_API sc_session
{
	int refcnt;

protected:
	/**
	 * Флаг, который указывает на то, что сессия закрыта.
	 */
	int dead;

	/**
	 * Путь к директории, в которой работает сессия.
	 */
	sc_string cwd;

	sc_wait_session_list good_waits;

public:
	/**
	 * @brief Последняя произошедшая ошибка.
	 */
	sc_retval error;

	#ifdef COUNT_ELEMENTS
		static int ses_cnt;
	#endif

protected:
	/**
	 * Множество открытых сегментов для данной сессии.
	 */
	segment_set opened_segments;

public:

	/**
	 * @brief Возвращает множество открытых сегментов в рамках данной сессии.
	 * @return множество открытых сегментов для данной сессии.
	 */
	const segment_set &get_segment_set() const
	{
		return opened_segments;
	}

	sc_session();

	virtual ~sc_session();

	/**
	 * @brief Захватить данную сессию. Увеличить счетчик ссылок.
	 */
	virtual sc_session *ref();

	/**
	 * @brief Освободить данную сессию. Уменьшить счетчик ссылок.
	 */
	virtual void unref();

	/**
	 * @brief Закрывает сессию.
	 */
	virtual void close() = 0;

	/**
	 * @brief Метод получения кода возврата последней проведенной операции.
	 */
	virtual sc_retval get_error() = 0;

	/**
	 * @brief Метод перемещает sc-элемент @p addr в сегмент @p to.
	 * @return Новый адрес перемещенного элемента.
	 */
	virtual sc_addr __move_element(sc_addr addr, sc_segment *to) = 0;



	/**
	 * @name Работа с сегментами и директориями.
	 * @{
	 */

	/**
	 * @brief Проверяет, открыт ли сегмент.
	 * @return true, если сегмент открыт в рамках данной сессии, иначе - false.
	 */
	virtual bool is_segment_opened(sc_segment *seg) = 0;

	/**
	 * @brief Создает сегмент по URI @p uri.
	 */
	virtual sc_segment *create_segment(const sc_string &uri) = 0;

	/**
	 * @brief Открывает уже существующий сегмент по URI @p uri.
	 * @return возвращает указатель на сегмент или нулевой указатель при не успешном завершении.
	 */
	virtual sc_segment *open_segment(const sc_string &uri) = 0;

	/**
	 * @brief Открывает сегмент по полному пути и сегменты, которые связаны с элементами лежащими в данном дугами, если установлен флаг @p complete,
	 * то метод потерпит неудачу, если хотя бы один сегмент невозможно открыть.
	 * @return возвращает указатель на сегмент или нулевой указатель при не успешном завершении.
	 */
	virtual sc_segment *open_segment_spider(const sc_string &uri, bool complete) = 0;

	/**
	 * @brief Проверяет является ли объект указанный в пути сегментом, либо это директория.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval _stat(const sc_string &uri) = 0;

	/**
	 * @brief Закрывает переданный в качестве параметра сегмент.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval close_segment(sc_segment *seg) = 0;

	/**
	 * @brief Закрывает переданный в качестве параметра сегмент по URI @p uri.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval close_segment(const sc_string &uri) = 0;

	/**
	 * @brief Удаляет переданный в качестве параметра сегмент.
	 */
	virtual sc_retval unlink(const sc_string &uri) = 0;

	/**
	 * @brief Изменяет имя сегмента.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval rename(const sc_string &oldname, const sc_string &newname) = 0;
	// TODO: link(), symlink() ?
	// TODO: mount(), umount() ?

	/**
	 * @brief Cоздает директорию.
	 * @param uri имя директории.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval mkdir(const sc_string &uri) = 0;

	// TODO: virtual sc_retval		rmdir(const sc_string &uri)=0;

	/**
	 * @brief Устанавливает текущей директорией переданную.
	 * @param uri URI директории.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	sc_retval chdir(const sc_string &uri);

	// TODO: sc_string			getcwd() {return cwd;}

	/**
	 * @brief Cоздает объект итератора по директории.
	 * @param uri URI директории.
	 * @return возвращает указатель на итератор по директории и 0 в противном случае.
	 */
	virtual sc_dir_iterator *search_dir(const sc_string &uri) = 0;

	/**
	 * @brief Возращает указатель на открытый сегмент.
	 * @param uri URI сегмента.
	 * @return возвращает указатель на сегмент, если сегмент найден и 0 в противном случае.
	 */
	virtual sc_segment *find_segment(const sc_string &uri) = 0;

	// }@


	/**
	 * @name Работа с sc-дугами.
	 */
	// @{

	/**
	 * @brief Позволяет получить sc-элемент являющийся концом sc-дуги, в том случае если переданный на вход методу sc-элемент являлся sc-дугой
	 */
	virtual sc_addr get_end(sc_addr) = 0;

	/**
	 * @brief Позволяет получить sc-элемент являющийся началом sc-дуги, в том случае если переданный на вход методу sc-элемент являлся sc-дугой.
	 */
	virtual sc_addr get_beg(sc_addr) = 0;

	/**
	 * @brief Позволяет установить начальный sc-элемент для sc-дуги.
	 */
	virtual sc_retval set_beg(sc_addr arc, sc_addr beg) = 0;

	/**
	 * @brief Позволяет установить конечный sc-элемент для sc-дуги.
	 */
	virtual sc_retval set_end(sc_addr arc, sc_addr end) = 0;

	/**
	 * @brief Позволяет получить sc-элемент являющийся началом sc-дуги, в том случае если переданный на вход методу sc-элемент являлся sc-дугой.
	 * @note Открывает сегмент начала sc-дуги, если он не открыт.
	 */
	virtual sc_addr get_beg_spin(sc_addr) = 0;

	/**
	 * @brief Позволяет получить sc-элемент являющийся концом sc-дуги, в том случае если переданный на вход методу sc-элемент являлся sc-дугой.
	 * @note Открывает сегмент конца sc-дуги, если он не открыт.
	 */
	virtual sc_addr get_end_spin(sc_addr) = 0;

	// }@

	/**
	 * @name Работа с итераторами.
	 */
	// @{

	/**
	 * @brief Создает объект итератора по указанному первым параметром шаблону.
	 * @param constr объект шаблона sc-конструкции.
	 * @param sink флаг, указывающий удалять объект шаблона, либо не удалять. true - удалять, false - не удалять.
	 * @return возвращает указатель на итератор по конструкции описанной шаблоном.
	 */
	virtual sc_iterator *create_iterator(sc_constraint *constr, bool sink) = 0;

	// @}

	/**
	 * @name Создание sc-элементов и sc-конструкций.
	 */
	// @{

	/**
	 * @brief Создает sc-элемент в сегменте @p seg c типом @p type.
	 * @return Если элемент создан, то возвращает его адрес и устанавливает error в #RV_OK.
	 * @return Если элемент не может быть создан, тогда возвращает 0 и устанавливает error в #RV_ERR_GEN.
	 */
	virtual sc_addr create_el(sc_segment *seg, sc_type type) = 0;

	/**
	 * @brief Генерирует 3-х элементную sc-конструкцию.
	 * @param e1 первый sc-элемент (должен быть создан).
	 * @param[out] e2 второй sc-элемент (создается и записывается в переданный параметр).
	 * @param seg2 сегмент, в котором будет создан второй sc-элемент.
	 * @param t2 тип второго sc-элемента.
	 * @param e3 третий sc-элемент (должен быть создан).
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval gen3_f_a_f(sc_addr e1, sc_addr *e2, sc_segment *seg2, sc_type t2, sc_addr e3) = 0;

	/**
	 * @brief Генерирует 5-х элементную sc-конструкцию.
	 * @param e1 первый sc-элемент (должен быть создан).
	 * @param[out] e2 второй sc-элемент (создается и записывается в переданный параметр).
	 * @param seg2 сегмент, в котором будет создан второй sc-элемент.
	 * @param t2 тип второго sc-элемента.
	 * @param e3 третий sc-элемент (должен быть создан).
	 * @param[out] e4 третий sc-элемент (создается и записывается в переданный параметр).
	 * @param seg4 сегмент, в котором будет создан третий sc-элемент.
	 * @param t4 тип третьего sc-элемента.
	 * @param e5 пятый sc-элемент (должен быть создан).
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval gen5_f_a_f_a_f(sc_addr e1, sc_addr *e2, sc_segment *seg2, sc_type t2, sc_addr e3, sc_addr *e4, sc_segment *seg4, sc_type t4, sc_addr e5) = 0;

	// @}

	/**
	 * @name Работа с событиями.
	 */
	// @{

	/**
	 * @brief Подписать @p wait на событие @p type c параметрами @p params в количестве @p len.
	 */
	virtual sc_retval attach_wait(sc_wait_type type, sc_param *params, int len, sc_wait*) = 0;

	virtual sc_retval attach_good_wait(sc_wait_type type, sc_param *, int len, sc_wait*) = 0;

	/**
	 * @brief Отменить подпись @p wait на все события.
	 */
	virtual sc_retval detach_wait(sc_wait *wait) = 0;

	// }@

	/**
	 * @name Работа с типом sc-элементов.
	 */
	// @{

	/**
	 * @brief Возвращает тип элемента @p el.
	 * @return Если элемент @p el доступен из данной сессии, то возвращает тип элемента и устанавливает session#error в #RV_OK.
	 * @return Если элемент @p el недоступен из данной сессии, то возвращает 0 и устанавливает session#error в #RV_ERR_GEN.
	 */
	virtual sc_type get_type(sc_addr el) = 0;

	/**
	 * @brief Позволяет изменить тип sc-элемента задаваемого переменной @p el на тип передаваемый во втором параметре.
	 * @param el указатель на sc-элемент.
	 * @param type тип sc-элемента.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval change_type(sc_addr el, sc_type type) = 0;

	// }@

	/**
	 * @brief Позволяет удалить sc-элемент.
	 * @return возвращает #RV_THEN, если операция прошла успешно, ELSE-код возврата - иначе (#RV_IS_ELSE).
	 */
	virtual sc_retval erase_el(sc_addr) = 0;

	/*! @name Working with idfts.
	 */
	// @{

	//! Returns identificator of SC-element.
	//! @note If SC-element hasn't identificator then returns system-generated.
	virtual sc_string get_idtf(const sc_addr addr) = 0;

	//! Sets identificator of SC-element.
	//! @return #RV_THEN if success, ELSE-sc_retval - else.
	virtual sc_retval set_idtf(sc_addr addr, const sc_string &idtf) = 0;

	//! Erase SC-element identificator.
	//! @return #RV_THEN if success, ELSE-sc_retval - else.
	virtual sc_retval erase_idtf(sc_addr addr) = 0;

	//! Finds SC-element by identificator in specified SC-segment.
	//! If not found then returns 0.
	virtual sc_addr find_by_idtf(const sc_string &idtf, sc_segment *seg) = 0;

	// @}

	/**
	 * @name Работа с содержимым sc-элементов.
	 */
	// @{

	/**
	 * @brief Позволяет получить содержимое sc-элемента, который задается первым параметром.
	 * @param addr адрес sc-элемента.
	 * @return возвращает объект класса Content.
	 */
	virtual Content get_content(sc_addr addr) = 0;

	/**
	 * @brief Позволяет получить содержимое sc-элемента, который задается первым параметром.
	 * @param addr адрес sc-элемента.
	 * @return возвращает объект класса Content.
	 */
	virtual const Content *get_content_const(sc_addr addr) = 0;

	/**
	 * @brief Позволяет установить содержимое sc-элемента, который задается первым параметром .
	 * @param addr адрес sc-элемента.
	 * @param content объект класса содержимого sc-элемента.
	 * @return возвращает THEN в случае успешного выполнения, ELSE- в противном случае.
	 */
	virtual sc_retval set_content(sc_addr addr, const Content &content) = 0;

	/**
	 * @brief Позволяет очистить содержимое sc-элемента, который задается первым параметром .
	 * @param addr адрес sc-элемента.
	 * @return возвращает THEN в случае успешного выполнения, ELSE- в противном случае.
	 */
	virtual sc_retval erase_content(sc_addr addr) = 0;

	/// Find sc-elements by content in all opened sc-segments.
	///
	/// @param content content for finding.
	/// @param[out] result to this container add founded sc-elements.
	/// @return true, if found at least one sc-element, else false.
	///
	/// @note Uses binary representation of content for search.
	/// @note Don't clear @p result.
	virtual bool find_by_content(const Content &content, addr_list &result) = 0;

	// @}

	/**
	 * @brief Метод получения количества выходящих дуг в sc-элемент @p addr.
	 * @return количество выходящих дуг.
	 */
	virtual int get_out_qnt(sc_addr addr) = 0;

	/**
	 * @brief Метод получения количества входящих дуг в sc-элемент @p addr.
	 * @return количество входящих дуг.
	 */
	virtual int get_in_qnt(sc_addr a) = 0;

	/**
	 * @brief Передает управление активности связанной с sc-элементов @p addr.
	 */
	virtual sc_retval activate(sc_addr addr, sc_addr param = 0, sc_addr prm2 = 0, sc_addr prm3 = 0) = 0;

	/**
	 * @brief Метод установки активности для sc-элемента с адресом @p addr.
	 */
	virtual sc_retval reimplement(sc_addr addr, sc_activity *new_activity) = 0;

	/**
	 * @brief Метод преобразования URI сегмента в знак сегмента.
	 */
	virtual sc_addr uri2sign(const sc_string &uri) = 0;

	/**
	 * @brief Метод преобразования знака сегмента в URI сегмента.
	 */
	virtual sc_string sign2uri(sc_addr addr) = 0;

	/**
	 * @brief Создает дочернюю сессию с такой же конфигурацией, как и у текущей.
	 */
	virtual sc_session *__fork() = 0;

	/**
	 * @brief Завершает транзакцию, начатую в рамках данной сессии.
	 */
	virtual void commit();

	/**
	 * @brief Откатывает транзакцию, начатую в рамках данной сессии.
	 */
	virtual void rollback();

	virtual bool erase_el_pre(sc_addr addr) = 0;
	virtual sc_retval __erase_el(sc_addr addr) = 0;
};

/**
 * @return обычную сессию.
 */
LIBSC_API sc_session *libsc_login();

/**
 * @brief Инициализирует liSC.
 * @param use_fs_repo флаг, который указывает на то, что необходимо загружать репозиторий с файловой системы.
 * @return системную сессию.
 */
LIBSC_API sc_session *libsc_init(int use_fs_repo = 0);

/**
 * @brief Деинициализирует liSC.
 */
LIBSC_API void libsc_deinit();

LIBSC_API void libsc_flush();

/**
 * @brief Системная сессия. Для нее все сегменты открыты.
 */
extern LIBSC_API sc_session *system_session;

/**
 * @brief Путь к репозиторию на файловой системе.
 */
extern LIBSC_API sc_string fs_repo_loc;

/// True value indicates what sc-memory doesn't throw error if detects
/// unresolved link while segment loading.
extern LIBSC_API bool allow_unresolved_links;

#endif // __LIBSC_H__

/**@}*/
