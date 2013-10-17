/* 
 * kcompress.c - Loadable Kernel Module for adding file compressing systemcall
 * Authors:
 * 	Jignesh Kumar Patel <Jignesh.Patel@umkc.edu>
 * 	Mitulkumar Patel    <mvpt33@umkc.edu>
 * 	Harshil Parikh      <happ67@umkc.edu>
 */

/* Header files */
#include "kcompress.h"	/* header file containing macros & includes */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jignesh <Jignesh.Patel@umkc.edu>, Mitul <mvpt33@umkc.edu> & Harshil <happ67@umkc.edu>");
MODULE_DESCRIPTION("This module implements Kernel level systemcall interposition");


/* Access the system call table (table of functions implementing the system 
 * calls). 
 */
extern void *sys_call_table[];

/* ZLIB */
asmlinkage int sys_kcompress(char *infile, char *outfile, int flags, int level);
asmlinkage int my_zlib_def(struct file *infilep, struct file *outfilep, int level);
asmlinkage int my_zlib_inf(struct file *infilep, struct file *outfilep, int level);

/* LZO */
asmlinkage int my_lzo_compress(struct file *infilep, struct file *outfilep);
asmlinkage int my_lzo_dcompress(struct file *infilep, struct file *outfilep);

/* initialize the kernel module to replace the system call */
int init_module()
{
	/* insert new system call */
	sys_call_table[__NR_sys_kcompress] = sys_kcompress;
	
	/* all set for kernel level system call tracing :) */	
	printk(KERN_ALERT "<-*-> KCompress system call active <-*->\n");
	printk(KERN_ALERT "      Please see the syslogd file /var/log/messages for complete output\n");
	
  return 0;
}

/* cleanup by restoring the original system call if module is removed */
void cleanup_module()
{
	printk(KERN_ALERT "<-*-> KCompress System call removed <-*->\n");
}


asmlinkage int sys_kcompress(char *infile, char *outfile, int flags, int level)
{
	static struct file *infilep;
	static struct file *outfilep;

	/* open the input file */
	infilep = filp_open(infile,O_RDONLY,0);
	if(IS_ERR(infilep))
	{
		printk(KERN_ALERT "Input file doesnot exist\n");
		return -1;
	}
	/* input file opened. Check whether it is a valid file */
	if(!(S_ISREG(infilep->f_dentry->d_inode->i_mode)))
	{
		printk(KERN_ALERT "Input file is not a regular file\n");
		return -1;
	}
	/* check whether filesystem supports read operation */
	if(infilep->f_op->read == 0)
	{
		printk(KERN_ALERT "Filesystem doesnot support read\n");
		return -1;
	}
	
	/* open the output file */
	outfilep=filp_open(outfile,O_CREAT|O_RDWR|O_EXCL,infilep->f_dentry->d_inode->i_mode);
	if(IS_ERR(outfilep))
	{
		printk(KERN_ALERT "Cannot open output file. Output file may exists.\n");
		return -1;
	}
	/* check whether filesystem supports write operation */
	if(infilep->f_op->write == 0)
	{
		printk(KERN_ALERT "Filesystem doesnot support write\n");
		return -1;
	}
	/* check whether inputfile and output file refer to same file */
	if(infilep->f_dentry->d_inode->i_ino==outfilep->f_dentry->d_inode->i_ino)
	{
		printk(KERN_ALERT "Output file same as input file\n");
		return -1;
	}
	/* read the flags */
	switch(flags)
	{
		case ZLIB_C:
			/* compress the input file using ZLIB */
			if(my_zlib_def(infilep,outfilep,level)!=Z_OK)
			{
				printk(KERN_ALERT "ZLIB compress error\n");
				return -1;
			}
			break;
		case ZLIB_D:
			if(my_zlib_inf(infilep,outfilep,level)!=Z_OK)
			{
				printk(KERN_ALERT "ZLIB decompress error\n");
				return -1;
			}
			break;
		case LZO_C:
			if(my_lzo_compress(infilep,outfilep)!=0)
			{
				printk(KERN_ALERT "LZO compress error\n");
				return -1;
			}
			break;
		case LZO_D:
			if(my_lzo_dcompress(infilep,outfilep)!=0)
			{
				printk(KERN_ALERT "LZO decompress error\n");
				return -1;
			}
			break;
		default:
			printk(KERN_ALERT "invalid flags in systemcall\n");
			return -1;
	}
	
	fput(infilep);
	fput(outfilep);	
	return 0;
}

asmlinkage int my_zlib_def(struct file *infilep, struct file *outfilep, int level)
{
	int ret; /* return value */
	int flush; /* flush to the output buffer or not */
	unsigned have; /* no. of bytes that HAVE been compressed */
	z_stream strm; /* stream datastructure for compressing */
	char *in; /* input buffer - stores uncompressed data */
	char *out; /* output buffer - stores compressed data */

	int rb,wb; /* rb - bytes read, wb - bytes written : to/from the file */
	
	/* initialize workspace */
	strm.workspace = vmalloc(zlib_deflate_workspacesize());
	
	/* initialize deflate */
	ret = zlib_deflateInit(&strm, level);
    
	/* check for initialization error */
	if (ret != Z_OK)
	{
		printk(KERN_ALERT"Error in initializing deflate \n");
		return ret;
	}

	/* allocate memory for input/output buffer */
	in = vmalloc(CHUNK);
	out = vmalloc(CHUNK);
	/* start reading ... from input */
	infilep->f_pos = 0; 
	/* start writing ... to output */
	outfilep->f_pos = 0;
	
	do {
		/* read from input file */
		rb = read_file(infilep, in, CHUNK);
		/* set the stream structure */
		strm.avail_in = rb;
		strm.next_in = in;
		
		/* set flush type */
		flush = (strm.avail_in != CHUNK)?Z_FINISH:Z_NO_FLUSH;

		/* run deflate */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = zlib_deflate(&strm,flush);
			have = CHUNK - strm.avail_out;
			wb = write_file(outfilep,out,have);
		}while(strm.avail_out == 0);
		
	}while(rb == CHUNK);
	
	/* end deflate */
	zlib_deflateEnd(&strm);
	vfree(in);
	vfree(out);
    return Z_OK;
}

asmlinkage int my_zlib_inf(struct file *infilep, struct file *outfilep, int level)
{
	
	int ret; /* return value */
	int flush; /* flush to the output buffer or not */
	unsigned have; /* no. of bytes that HAVE been compressed */
	z_stream strm; /* stream datastructure for compressing */
	char *in; /* input buffer - stores uncompressed data */
	char *out; /* output buffer - stores compressed data */

	int rb,wb; /* rb - bytes read, wb - bytes written : to/from the file */
	
	/* initialize workspace */
	strm.workspace = vmalloc(zlib_inflate_workspacesize());
	
	/* initialize inflate */
	ret = zlib_inflateInit(&strm);
    
	/* check for initialization error */
	if (ret != Z_OK)
	{
		printk(KERN_ALERT"Error in initializing inflate \n");
		return ret;
	}

	/* allocate memory for input/output buffer */
	in = vmalloc(CHUNK);
	/* making output buffer large to handle overflow */
	out = vmalloc(CHUNK*5);
	/* start reading ... from input */
	infilep->f_pos = 0; 
	/* start writing ... to output */
	outfilep->f_pos = 0;
	
	do {
		rb = read_file(infilep, in, CHUNK);
		strm.avail_in = rb;
		printk(KERN_ALERT"reading %d bytes from i/p file\n",rb);
		flush = (rb != CHUNK)?Z_FINISH:Z_NO_FLUSH;
		strm.next_in = in;

		/* run inflate */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = zlib_inflate(&strm,flush);
			have = CHUNK - strm.avail_out;
			wb = write_file(outfilep,out,have);
		}while(strm.avail_out == 0);
	}while(ret != Z_STREAM_END);

	/* end inflate */
	zlib_inflateEnd(&strm);
	vfree(in);
	vfree(out);
    return Z_OK;
}

asmlinkage int my_lzo_compress(struct file *infilep, struct file *outfilep)
{
	int ret = 0;	
	lzo_byte *in = NULL;
	lzo_byte *out = NULL;
	lzo_byte *wrkmem = NULL;
	lzo_uint in_len;
	lzo_uint out_len;
	lzo_uint32 wrk_len = 0;
	
	lzo_uint block_size=4096;

	int len32=4;	
	
	/* allocate compression buffer and work memory */
	in = vmalloc(block_size);
	out = vmalloc(block_size + block_size / 64 + 16 + 3);
	wrk_len = LZO1X_1_MEM_COMPRESS;
	wrkmem = vmalloc(wrk_len);

	if(in == NULL || out == NULL || wrkmem == NULL)
	{
		printk(KERN_ALERT"vmalloc error");
		ret = -1;
		return ret;
	}
	
	/* start reading ... from input */
	infilep->f_pos = 0; 
	/* start writing ... to output */
	outfilep->f_pos = 0;
	/* process blocks */
	
	for(;;)
	{
		/* read block */
		in_len = read_file(infilep, in, CHUNK);
		if(in_len<=0) break;

		/* compress block */
		ret = lzo1x_1_compress(in,in_len,out,&out_len,wrkmem);
	        if(ret != LZO_E_OK)// || out_len > in_len /64 + 16 +3)
		{
			printk(KERN_ALERT"LZO_E_NOT OK");
			ret=-1;
			goto error;
		}	
		
		/* write uncompressed block size */
		write_file(outfilep,&in_len,len32);
		
		if(out_len<in_len)
		{
			/* write compressed block */
			write_file(outfilep,&out_len,len32);
			write_file(outfilep,out,out_len);
		}
		else
		{
			/* write uncompressed block */
			write_file(outfilep,&in_len,len32);
			write_file(outfilep,in,in_len);
		}
	}
	/* mark end of file */
	write_file(outfilep, 0,len32);
	
	ret = 0;
error:
	vfree(wrkmem);
	vfree(out);
	vfree(in);
    return ret;
}

asmlinkage int my_lzo_dcompress(struct file *infilep, struct file *outfilep)
{
	int ret = 0;
	lzo_byte *buf = NULL;
	lzo_uint buf_len;
	
	lzo_byte *in;
	lzo_byte *out;
	lzo_uint in_len;
	lzo_uint out_len;
	lzo_uint block_size=4096;

	int len32 = 4;

	/* allocate buffer for decompression */
	buf_len = block_size + block_size / 64 + 16 + 3;
	buf = vmalloc(buf_len);

	if(buf == NULL)
	{
		printk(KERN_ALERT"vmalloc error");
		ret = -1;
		return ret;
	}
	/* process blocks */
	for(;;)
	{
		/* read uncompressed size */
		read_file(infilep,&out_len,len32);
	
		/* exit if last block */
		if(out_len == 0)  break;

		/* read compressed size */
		read_file(infilep,&in_len,len32);

		/* check for errors */
		if(in_len >block_size || out_len > block_size || in_len ==0
				|| in_len > out_len)
		{
			ret = -1;
			goto error;
		}

		/* place compressed block at top */
		in = buf + buf_len - in_len;
		out = buf;

		/* read compressed block */
		in_len = read_file(infilep, in, in_len);
		if(in_len < out_len)
		{
			/* decompress */
			lzo_uint new_len = out_len;

			ret = lzo1x_decompress_safe(in,in_len,out,&new_len,NULL);

			if(new_len !=out_len)
				goto final;

			/* write decompressed data */
			write_file(outfilep,out,out_len);
		}
		else
		{
			/* write original (incompressible) block */
			write_file(outfilep,in,in_len);
		}
	}
final:
	ret = 0;
error:
	vfree(buf);
    return ret;
}
