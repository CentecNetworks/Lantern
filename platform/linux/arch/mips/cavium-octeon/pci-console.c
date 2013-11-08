/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006-2010 Cavium Networks
 */
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>

#include <asm/octeon/octeon.h>
#include <asm/octeon/octeon-pci-console.h>

struct pci_console_state {
	struct console con;
	struct tty_driver *ttydrv;
	struct timer_list poll_timer;
	int open_count;
	int index;
};

static struct pci_console_state pci_console_state;
static DEFINE_SPINLOCK(pci_console_lock);
static u64 pci_console_base_address;

/**
 * get_state_con -- get the pci console state from a struct console
 *
 * @con:    struct console to get console for
 *
 * Returns The console state
 */
static struct pci_console_state  *get_state_con(struct console *con)
{
	return (struct pci_console_state  *)con->data;
}

/**
 * get_state_tty -- get the pci console state from a tty
 *
 * @tty:    tty to get console for
 *
 * Returns The console state
 */
static struct pci_console_state  *get_state_tty(struct tty_struct *tty)
{
	return (struct pci_console_state  *)tty->driver->driver_state;
}

/**
 * pci_console_lowlevel_write -- low level kernel write to the PCI console
 *
 * @console_num:
 *               Console to write to
 * @str:    String to write
 * @len:    Length of the string
 */
static void pci_console_lowlevel_write(int console_num, const char *str,
				       unsigned len)
{
	unsigned long flags;
	spin_lock_irqsave(&pci_console_lock, flags);
	while (len > 0) {
		int written =
			octeon_pci_console_write(pci_console_base_address,
						 console_num, str, len,
						 OCT_PCI_CON_FLAG_NONBLOCK);
		if (written > 0) {
			str += written;
			len -= written;
		}
	}
	spin_unlock_irqrestore(&pci_console_lock, flags);
}

/**
 * pci_console_write -- kernel write to the PCI console
 *
 * @con:    Console to write to
 * @str:    String to write
 * @len:    Length of the string
 */
static void pci_console_write(struct console *con, const char *str,
			      unsigned len)
{
	pci_console_lowlevel_write(get_state_con(con)->index, str, len);
}

/**
 * pci_console_device -- get a TTY driver for the console device.
 *
 * Used to allow userspace to write to the kernel's console.
 *
 * @con:    Kernel's console
 * @index:  Which console index
 * Returns TTY driver for userspace. NULL on failure.
 */
static struct tty_driver *pci_console_device(struct console *con, int *index)
{
	struct pci_console_state  *console_state = get_state_con(con);
	*index = 0;
	return console_state->ttydrv;
}

/**
 * pci_console_setup -- called by the core when the console=string is parsed,
 *
 * @con:    Kernel's console
 * @arg:    Argument string
 * Returns Zero on success
 */
static int pci_console_setup(struct console *con, char *arg)
{
	octeon_write_lcd("pci cons");
	if (pci_console_base_address == 0) {
		const struct cvmx_bootmem_named_block_desc *block_desc =
			cvmx_bootmem_find_named_block(OCTEON_PCI_CONSOLE_BLOCK_NAME);
		if (block_desc == NULL) {
			octeon_write_lcd("pci fail");
			return -1;
		}
		pci_console_base_address = block_desc->base_addr;
	}
	return 0;
}

/**
 * pci_console_init -- initialize the PCI console for use
 */
void pci_console_init(const char *arg)
{
	memset(&pci_console_state, 0, sizeof(pci_console_state));
	strcpy(pci_console_state.con.name, "pci");
	pci_console_state.con.write = pci_console_write;
	pci_console_state.con.device = pci_console_device;
	pci_console_state.con.setup = pci_console_setup;
	pci_console_state.con.data = &pci_console_state;
	if (arg && (arg[3] >= '0') && (arg[3] <= '9'))
		sscanf(arg + 3, "%d", &pci_console_state.index);
	else
		pci_console_state.index = 0;
	register_console(&pci_console_state.con);
}

/**
 * pci_tty_read_poll -- called by a timer to poll the PCI device for input data
 *
 * @arg:    Pointer to the TTY structure
 */
static void pci_tty_read_poll(unsigned long arg)
{
	struct tty_struct *tty = (struct tty_struct *) arg;
	int index = get_state_tty(tty)->index;
	unsigned long flags;
	int count;
	spin_lock_irqsave(&pci_console_lock, flags);
	count = octeon_pci_console_read_avail(pci_console_base_address, index);
	if (count > 0) {
		char buffer[count];
		count = octeon_pci_console_read(pci_console_base_address, index,
						buffer, sizeof(buffer),
						OCT_PCI_CON_FLAG_NONBLOCK);
		tty_insert_flip_string(tty, buffer, count);
		tty_flip_buffer_push(tty);
	}
	spin_unlock_irqrestore(&pci_console_lock, flags);
	mod_timer(&get_state_tty(tty)->poll_timer, jiffies + 1);
}

/**
 * pci_tty_open -- called when userspace opens the TTY device.
 *
 * Can be called multiple times.
 *
 * @tty:    Device to open
 * @filp:
 * Returns Zero on success
 */
static int pci_tty_open(struct tty_struct *tty, struct file *filp)
{
	struct pci_console_state  *console_state = get_state_tty(tty);
	console_state->open_count++;
	if (console_state->open_count == 1) {
		init_timer(&console_state->poll_timer);
		console_state->poll_timer.data = (unsigned long) tty;
		console_state->poll_timer.function = pci_tty_read_poll;
		mod_timer(&console_state->poll_timer, jiffies + 1);
	}
	return 0;
}

/**
 * pci_tty_close -- called when userspace closes the console TTY
 *
 * @tty:    TTY to close
 * @filp:
 */
static void pci_tty_close(struct tty_struct *tty, struct file *filp)
{
	struct pci_console_state  *console_state = get_state_tty(tty);
	console_state->open_count--;
	if (console_state->open_count == 0)
		del_timer(&console_state->poll_timer);
}

/**
 * pci_tty_write -- called when usersapce does a block write.
 *
 * @tty:    TTY to write too
 * @buf:    Data to write
 * @count:  number of bytes
 * Returns Number of bytes written
 */
static int pci_tty_write(struct tty_struct *tty, const unsigned char *buf,
			 int count)
{
	pci_console_lowlevel_write(get_state_tty(tty)->index, buf, count);
	return count;
}

/**
 * pci_tty_send_xchar -- write a single character
 *
 * @tty:    TTY to write to
 * @ch:     Character to write
 */
static void pci_tty_send_xchar(struct tty_struct *tty, char ch)
{
	pci_console_lowlevel_write(get_state_tty(tty)->index, &ch, 1);
}

/**
 * pci_tty_write_room -- determine the amount of room available for output.
 *
 * @tty:    TTY structure
 * Returns Number of bytes
 */
static int pci_tty_write_room(struct tty_struct *tty)
{
	unsigned long flags;
	int count;
	spin_lock_irqsave(&pci_console_lock, flags);
	count = octeon_pci_console_write_avail(pci_console_base_address,
					       get_state_tty(tty)->index);
	spin_unlock_irqrestore(&pci_console_lock, flags);
	if (count)
		return count;
	else
		return 0;
}

/**
 * pci_tty_chars_in_buffer -- return the number of characters pending.
 *
 * Needed for vi to work.
 *
 * @tty:    TTY structure
 *
 * Returns Number of bytes
 */
static int pci_tty_chars_in_buffer(struct tty_struct *tty)
{
	return 0;
}

static struct tty_operations octeon_pci_tty_ops = {
	.open = pci_tty_open,
	.close = pci_tty_close,
	.write = pci_tty_write,
	.write_room = pci_tty_write_room,
	.send_xchar = pci_tty_send_xchar,
	.chars_in_buffer = pci_tty_chars_in_buffer,
};

static __init int pci_console_module_init(void)
{
	pci_console_state.ttydrv = alloc_tty_driver(1);
	if (!pci_console_state.ttydrv)
		return 0;

	pci_console_state.ttydrv->owner = THIS_MODULE;
	pci_console_state.ttydrv->driver_name = "pci_console";
	pci_console_state.ttydrv->name = "ttyPCI";
	pci_console_state.ttydrv->type = TTY_DRIVER_TYPE_SERIAL;
	pci_console_state.ttydrv->subtype = SERIAL_TYPE_NORMAL;
	pci_console_state.ttydrv->flags = TTY_DRIVER_REAL_RAW;
	pci_console_state.ttydrv->major = 4;
	pci_console_state.ttydrv->minor_start = 96;
	pci_console_state.ttydrv->init_termios = tty_std_termios;
	pci_console_state.ttydrv->init_termios.c_cflag =
		B9600 | CS8 | CREAD | HUPCL | CLOCAL;
	pci_console_state.ttydrv->driver_state = &pci_console_state;
	tty_set_operations(pci_console_state.ttydrv, &octeon_pci_tty_ops);
	tty_register_driver(pci_console_state.ttydrv);
	return 0;
}
module_init(pci_console_module_init);
