/*
 * Author: Benedict R. Gaster
 * Module: Advanced Systems Programming
 *
 * Derived from the excellent blog post: https://graphitemaster.github.io/fibers/
 */
#pragma once
struct Context
{
  void *rip, *rsp;
  void *rbx, *rbp, *r12, *r13, *r14, *r15;
};

extern "C" int get_context(Context *c);
extern "C" void set_context(Context *c);
extern "C" void swap_context(Context *out, Context *in);