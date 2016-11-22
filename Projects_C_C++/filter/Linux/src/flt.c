#include <stdio.h>
#include <stdlib.h>
#include "gtest/gtest.h"

typedef enum standard_err
{
	OK,
	WAITING,
	ERR_ALLOCATION,
	ERR_FREE,
	ERR_ANOTHER,
	ERR_INVALIDVALUE,
} standard_err;

typedef struct r_buff_unit
{
	struct r_buff_unit* left;
	struct r_buff_unit* right;
	double unit;
} r_buff_unit;


standard_err ring_buffer_alloc(int size, r_buff_unit** out)
{
	standard_err ret = OK;
	*out = (r_buff_unit*)malloc(sizeof(r_buff_unit)); if (out == NULL) ret = ERR_ALLOCATION;
	r_buff_unit* tmp1 = NULL;
	r_buff_unit* tmp2 = *out;
	int i;
	for (i = 1; i < size; i++)
	{
		tmp1 = (r_buff_unit*)malloc(sizeof(r_buff_unit)); if (tmp1 == NULL) ret = ERR_ALLOCATION;
		tmp1->left = tmp2;
		tmp2->right = tmp1;
		tmp2 = tmp1;
	}
	tmp2->right = *out;
	(*out)->left = tmp2;
	return ret;
}

standard_err ring_buffer_delete(int size, r_buff_unit* out)
{
	standard_err ret = OK;
	r_buff_unit* tmp1 = out;
	r_buff_unit* tmp2 = out;
	int i;
	for (i = 0; i < size; i++)
	{
		free(tmp1);
		tmp1 = tmp2->right;
		tmp2 = tmp1;
	}
	return ret;
}

standard_err ring_buffer_put(double put, r_buff_unit* in)
{
	standard_err ret = OK;
	r_buff_unit* tmp = NULL;
	tmp = in->right;
	tmp->unit = put;
	return ret;
}

typedef enum Bool {
	FALSE,
	TRUE
} Bool;


typedef struct core_ctx
{
	double* data;
	int size_core;
	int position;
	int size_buffer;
	Bool posibility;
	r_buff_unit* ptr;

}core_ctx;


double conv(double* s1, r_buff_unit* s2, int size)
{
	double tmp = 0.0;
	r_buff_unit* tmp_buff = s2;
	int i;
	for (i = 0; i < size; i++)
	{
		tmp += s1[size - i - 1] * tmp_buff->unit;
		tmp_buff = tmp_buff->left;
	}
	return tmp;
}



standard_err filter(void* core_info, double in, double* out)
{
	standard_err ret = OK;
	core_ctx* ctx = (core_ctx*)core_info;
	ctx->size_buffer++;
	ctx->ptr->unit = in;
	if (ctx->size_buffer < ctx->size_core)
	{
		ctx->ptr = ctx->ptr->right;
		ret = WAITING;
	}
	else
	{
		ctx->posibility = TRUE;
		*out = conv(ctx->data, ctx->ptr, ctx->size_core);
		ctx->ptr = ctx->ptr->right;
	}
	return ret;
}


TEST (Filtering_functio_test, PositiveNos) {

	core_ctx* main_ctx = (core_ctx*)malloc(sizeof(core_ctx));
	main_ctx->data = (double*)malloc(sizeof(double) * 3);
	main_ctx->data[0] = 1;
	main_ctx->data[1] = 1;
	main_ctx->data[2] = 1;
	main_ctx->posibility = FALSE;
	main_ctx->size_buffer = 0;
	main_ctx->size_core = 3;

	standard_err errors = OK;
	errors = ring_buffer_alloc(main_ctx->size_core, &main_ctx->ptr);
	float tmp;
	double out = 0;
	tmp = 1;
    EXPECT_EQ (WAITING, filter(main_ctx, tmp, &out));
    tmp = 1;
    EXPECT_EQ (WAITING, filter(main_ctx, tmp, &out));
    filter(main_ctx, tmp, &out);
    EXPECT_EQ (3, out);
    filter(main_ctx, 5, &out);
    EXPECT_EQ (7, out);
    filter(main_ctx, 100, &out);
    EXPECT_EQ (102, out);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

