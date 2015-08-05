/*@
XOC Release License

Copyright (c) 2013-2014, Alibaba Group, All rights reserved.

    compiler@aliexpress.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Su Zhenyu nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

author: Su Zhenyu
@*/
#ifndef __OPTION_H__
#define __OPTION_H__

class PASS_MGR;

#define NO_OPT        0
#define OPT_LEVEL1    1
#define OPT_LEVEL2    2
#define OPT_LEVEL3    3
#define SIZE_OPT      4

//Optimization Context
//This class record and propagate auxiliary information to optimizations.
#define OPTC_is_ref_valid(o)				((o).u1.s1.is_du_ref_valid)
#define OPTC_is_du_chain_valid(o)			((o).u1.s1.is_du_chain_valid)
#define OPTC_is_live_expr_valid(o)			((o).u1.s1.is_live_expr_valid)
#define OPTC_is_reach_def_valid(o)			((o).u1.s1.is_reach_def_valid)
#define OPTC_is_avail_reach_def_valid(o)	((o).u1.s1.is_avail_reach_def_valid)
#define OPTC_is_cfg_valid(o)				((o).u1.s1.is_cfg_valid)
#define OPTC_is_aa_valid(o)					((o).u1.s1.is_aa_result_valid)
#define OPTC_is_expr_tab_valid(o)			((o).u1.s1.is_ir_expr_tab)
#define OPTC_is_cdg_valid(o)				((o).u1.s1.is_cdg_valid)
#define OPTC_is_dom_valid(o)				((o).u1.s1.is_dom_valid)
#define OPTC_is_pdom_valid(o)				((o).u1.s1.is_pdom_valid)
#define OPTC_is_rpo_valid(o)				((o).u1.s1.is_rpo_valid)
#define OPTC_is_loopinfo_valid(o)			((o).u1.s1.is_loopinfo_valid)
#define OPTC_is_callg_valid(o)				((o).u1.s1.is_callg_valid)
#define OPTC_show_comp_time(o)				((o).u2.s1.show_compile_time)
#define OPTC_pass_mgr(o)					((o).pass_mgr)
class OPT_CTX {
public:
	union {
		UINT int1;
		struct {
			//Record MUST-DEF, MAY-DEF, MAY-USE MD_SET for each IR STMT/EXP.
			UINT is_du_ref_valid:1;

			UINT is_du_chain_valid:1; //Record DEF, USE IR LIST for IR STMT.
			UINT is_live_expr_valid:1;
			UINT is_reach_def_valid:1;
			UINT is_avail_reach_def_valid:1;
			UINT is_aa_result_valid:1; //POINT TO info is avaiable.
			UINT is_ir_expr_tab:1; //Liveness of IR_EXPR is avaliable.
			UINT is_cfg_valid:1; //CFG is avaliable.
			UINT is_cdg_valid:1; //CDG is avaliable.

			//Dominator Set, Immediate Dominator are avaliable.
			UINT is_dom_valid:1;

			//Post Dominator Set, Post Immediate Dominator are avaiable.
			UINT is_pdom_valid:1;

			UINT is_loopinfo_valid:1; //Loop info is avaiable.

			UINT is_callg_valid:1; //Call graph is available.

			UINT is_rpo_valid:1; //Rporder is available.
		} s1;
	} u1;

	union {
		UINT int1;
		struct {
			UINT show_compile_time:1; //Show compilation time.
		} s1;
	} u2;

	PASS_MGR * pass_mgr; //indicate the pass manager.
public:
	OPT_CTX()
	{
		set_all_invalid();
		u2.int1 = 0;
		pass_mgr = NULL;
	}

	void set_all_valid() { u1.int1 = (UINT)-1; }
	void set_all_invalid() { u1.int1 = 0; }

	//This function reset the flag if control flow changed.
	void set_flag_if_cfg_changed()
	{
		OPTC_is_cfg_valid(*this) = false;
		OPTC_is_cdg_valid(*this) = false;
		OPTC_is_dom_valid(*this) = false;
		OPTC_is_pdom_valid(*this) = false;
		OPTC_is_rpo_valid(*this) = false;
		OPTC_is_loopinfo_valid(*this) = false;
	}

	inline bool is_all_intra_valid()
	{
		return OPTC_is_ref_valid(*this) &&
				OPTC_is_du_chain_valid(*this) &&
				OPTC_is_live_expr_valid(*this) &&
				OPTC_is_reach_def_valid(*this) &&
				OPTC_is_avail_reach_def_valid(*this) &&
				OPTC_is_aa_valid(*this) &&
				OPTC_is_expr_tab_valid(*this) &&
				OPTC_is_dom_valid(*this) &&
				OPTC_is_pdom_valid(*this) &&
				OPTC_is_cfg_valid(*this) &&
				OPTC_is_cdg_valid(*this) &&
				OPTC_is_rpo_valid(*this) &&
				OPTC_is_loopinfo_valid(*this);
	}
};

//Declare the optimization.
typedef enum _OPT_TYPE {
	OPT_UNDEF = 0,
	OPT_CFG,
	OPT_AA,
	OPT_CP,
	OPT_CCP,
	OPT_GCSE,
	OPT_LCSE,
	OPT_RP,
	OPT_PRE,
	OPT_IVR,
	OPT_LICM,
	OPT_DCE,
	OPT_DSE,
	OPT_RCE,
	OPT_GVN,
	OPT_DOM,
	OPT_PDOM,
	OPT_DU_REF,
	OPT_LIVE_EXPR,
	OPT_AVAIL_REACH_DEF,
	OPT_DU_CHAIN,
	OPT_EXPR_TAB,
	OPT_LOOP_INFO,
	OPT_CDG,
	OPT_LOOP_CVT,
	OPT_RPO,
	OPT_POLY,
	OPT_PRDF,
	OPT_VRP,
	OPT_IPA,
	OPT_SSA_MGR,
	OPT_CFS_MGR,
	OPT_POLY_TRAN,
	OPT_NUM,
} OPT_TYPE;

//Exported Variables
extern CHAR * g_func_or_bb_option;
extern INT g_opt_level;
extern bool g_do_gra;
extern bool g_do_refine;
extern bool g_do_refine_auto_insert_cvt;

extern bool g_is_hoist_type; //Hoist data type from less than INT to INT.
extern bool g_do_ipa;
extern bool g_show_comp_time;
extern bool g_do_inline;
extern UINT g_inline_threshold;
extern bool g_is_opt_float; //Optimize float point operation.
extern bool g_is_lower_to_simplest; //Lower IR to simplest form.
extern bool g_do_ssa; //Do optimization in SSA.
extern bool g_do_cfg;
extern bool g_do_rpo;
extern bool g_do_loop_ana; //loop analysis.
extern bool g_do_cfg_remove_empty_bb;
extern bool g_do_cfg_remove_unreach_bb;
extern bool g_do_cfg_remove_tramp_bb;
extern bool g_do_cfg_dom;
extern bool g_do_cfg_pdom;
extern bool g_do_cdg;
extern bool g_do_aa;
extern bool g_do_du_ana;
extern bool g_do_compute_available_exp;
extern bool g_do_expr_tab;

extern bool g_do_dce;

/* Set true to eliminate control-flow-structures.
Note this option may incur user unexpected result:
e.g: If user is going to write a dead cyclic loop,
	void non_return()
	{
		for(;;) {}
	}
Aggressive DCE will remove the above dead cycle. */
extern bool g_do_dce_aggressive;

extern bool g_do_cp_aggressive; //It may cost much compile time.
extern bool g_do_cp;
extern bool g_do_rp;
extern bool g_do_gcse;
extern bool g_do_lcse;
extern bool g_do_pre;
extern bool g_do_rce;
extern bool g_do_dse;
extern bool g_do_licm;
extern bool g_do_ivr;
extern bool g_do_gvn;
extern bool g_do_cfs_opt;
extern bool g_build_cfs;
extern bool g_cst_bb_list; //Construct BB list.
extern UINT g_thres_opt_ir_num;
extern UINT g_thres_opt_bb_num;
extern bool g_do_loop_convert;
extern bool g_do_poly_tran;
#endif