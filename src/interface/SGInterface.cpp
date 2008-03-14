#include "lib/config.h"

#if !defined(HAVE_SWIG)

#include <string.h>
#include <stdlib.h>

#include "interface/SGInterface.h"
#include "lib/ShogunException.h"
#include "lib/Mathematics.h"
#include "structure/DynProg.h"
#include "guilib/GUICommands.h"
#include "gui/TextGUI.h"
#include "gui/GUI.h"
#include "classifier/svm/SVM.h"
#include "kernel/WeightedDegreePositionStringKernel.h"
#include "kernel/WeightedDegreeStringKernel.h"
#include "kernel/CommWordStringKernel.h"
#include "kernel/WeightedCommWordStringKernel.h"
#include "kernel/LinearKernel.h"
#include "kernel/SparseLinearKernel.h"
#include "kernel/CombinedKernel.h"
#include "kernel/CustomKernel.h"


CSGInterface* interface=NULL;
extern CTextGUI* gui;

#define USAGE(method) "sg('" method "')"
#define USAGE_I(method, in) "sg('" method "', " in ")"
#define USAGE_O(method, out) "[" out "]=sg('" method "')"
#define USAGE_IO(method, in, out) "[" out "]=sg('" method "', " in ")"

static CSGInterfaceMethod sg_methods[]=
{
	{
		(CHAR*) N_CRC,
		(&CSGInterface::a_crc),
		(CHAR*) USAGE_IO(N_CRC, "string", "crc32")
	},
	{
		(CHAR*) N_TRANSLATE_STRING,
		(&CSGInterface::a_translate_string),
		(CHAR*) USAGE_IO(N_TRANSLATE_STRING,
			"string, order, start", "translation")
	},
	{
		(CHAR*) N_BEST_PATH_2STRUCT,
		(&CSGInterface::a_best_path_2struct),
		(CHAR*) USAGE_IO(N_BEST_PATH_2STRUCT,
			"p, q, a_trans, seq, pos, genestr, penalties, penalty_info, nbest, dict_weights, segment_sum_weights",
			"prob, path, pos")
	},
	{
		(CHAR*) N_BEST_PATH_TRANS,
		(&CSGInterface::a_best_path_trans),
		(CHAR*) USAGE_IO(N_BEST_PATH_TRANS,
			"p, q, a_trans, seq, pos, orf_info, genestr, penalties, state_signals, penalty_info, nbest, dict_weights, use_orf, mod_words [, segment_loss, segmend_ids_mask]",
			"prob, path, pos")
	},
	{
		(CHAR*) N_BEST_PATH_TRANS_DERIV,
		(&CSGInterface::a_best_path_trans_deriv),
		(CHAR*) USAGE_IO(N_BEST_PATH_TRANS_DERIV,
			"my_path, my_pos, p, q, a_trans, seq, pos, genestr, penalties, state_signals, penalty_info, dict_weights, mod_words [, segment_loss, segmend_ids_mask]",
			"p_deriv, q_deriv, a_deriv, penalties_deriv, my_scores, my_loss")
	},
	{
		(CHAR*) N_BEST_PATH_NO_B,
		(&CSGInterface::a_best_path_no_b),
		(CHAR*) USAGE_IO(N_BEST_PATH_NO_B, "p, q, a, max_iter", "prob, path")
	},
	{
		(CHAR*) N_BEST_PATH_TRANS_SIMPLE,
		(&CSGInterface::a_best_path_trans_simple),
		(CHAR*) USAGE_IO(N_BEST_PATH_TRANS_SIMPLE,
			"p, q, a_trans, seq, nbest", "prob, path")
	},
	{
		(CHAR*) N_BEST_PATH_NO_B_TRANS,
		(&CSGInterface::a_best_path_no_b_trans),
		(CHAR*) USAGE_IO(N_BEST_PATH_NO_B_TRANS,
			"p, q, a_trans, max_iter, nbest", "prob, path")
	},
	{
		(CHAR*) N_GET_VERSION,
		(&CSGInterface::a_get_version),
		(CHAR*) USAGE_O(N_GET_VERSION, "version")
	},
	{
		(CHAR*) N_SET_LABELS,
		(&CSGInterface::a_set_labels),
		(CHAR*) USAGE_I(N_SET_LABELS, "'TRAIN|TEST', labels")
	},
	{
		(CHAR*) N_GET_LABELS,
		(&CSGInterface::a_get_labels),
		(CHAR*) USAGE_IO(N_GET_LABELS, "'TRAIN|TEST'", "labels")
	},
	{
		(CHAR*) N_FROM_POSITION_LIST,
		(&CSGInterface::a_obtain_from_position_list),
		(CHAR*) USAGE_I(N_FROM_POSITION_LIST, "'TRAIN|TEST', winsize, shift[, skip]")
	},
	{
		(CHAR*) N_GET_FEATURES,
		(&CSGInterface::a_get_features),
		(CHAR*) USAGE_IO(N_GET_FEATURES, "'TRAIN|TEST'", "features")
	},
	{
		(CHAR*) N_ADD_FEATURES,
		(&CSGInterface::a_add_features),
		(CHAR*) USAGE_O(N_ADD_FEATURES,
			"'TRAIN|TEST', features[, DNABINFILE|<ALPHABET>]")
	},
	{
		(CHAR*) N_SET_FEATURES,
		(&CSGInterface::a_set_features),
		(CHAR*) USAGE_O(N_SET_FEATURES,
			"'TRAIN|TEST', features[, DNABINFILE|<ALPHABET>]")
	},
	{
		(CHAR*) N_GET_DISTANCE_MATRIX,
		(&CSGInterface::a_get_distance_matrix),
		(CHAR*) USAGE_O(N_GET_DISTANCE_MATRIX, "D")
	},
	{
		(CHAR*) N_GET_KERNEL_MATRIX,
		(&CSGInterface::a_get_kernel_matrix),
		(CHAR*) USAGE_O(N_GET_KERNEL_MATRIX, "K")
	},
	{
		(CHAR*) N_SET_CUSTOM_KERNEL,
		(&CSGInterface::a_set_custom_kernel),
		(CHAR*) USAGE_I(N_SET_CUSTOM_KERNEL, "kernelmatrix, 'DIAG|FULL|FULL2DIAG'")
	},
	{
		(CHAR*) N_SET_WD_POS_WEIGHTS,
		(&CSGInterface::a_set_WD_position_weights),
		(CHAR*) USAGE_I(N_SET_WD_POS_WEIGHTS, "W[, 'TRAIN|TEST']")
	},
	{
		(CHAR*) N_SET_SUBKERNEL_WEIGHTS,
		(&CSGInterface::a_set_subkernel_weights),
		(CHAR*) USAGE_I(N_SET_SUBKERNEL_WEIGHTS, "W")
	},
	{
		(CHAR*) N_SET_SUBKERNEL_WEIGHTS_COMBINED,
		(&CSGInterface::a_set_subkernel_weights_combined),
		(CHAR*) USAGE_I(N_SET_SUBKERNEL_WEIGHTS_COMBINED, "W, idx")
	},
	{
		(CHAR*) N_SET_LAST_SUBKERNEL_WEIGHTS,
		(&CSGInterface::a_set_last_subkernel_weights),
		(CHAR*) USAGE_I(N_SET_LAST_SUBKERNEL_WEIGHTS, "W")
	},
	{
		(CHAR*) N_GET_SPEC_CONSENSUS,
		(&CSGInterface::a_get_SPEC_consensus),
		(CHAR*) USAGE_O(N_GET_SPEC_CONSENSUS, "W")
	},
	{
		(CHAR*) N_GET_SPEC_SCORING,
		(&CSGInterface::a_get_SPEC_scoring),
		(CHAR*) USAGE_IO(N_GET_SPEC_SCORING, "max_order", "W")
	},
	{
		(CHAR*) N_GET_WD_CONSENSUS,
		(&CSGInterface::a_get_WD_consensus),
		(CHAR*) USAGE_O(N_GET_WD_CONSENSUS, "W")
	},
	{
		(CHAR*) N_COMPUTE_POIM_WD,
		(&CSGInterface::a_compute_POIM_WD),
		(CHAR*) USAGE_IO(N_COMPUTE_POIM_WD, "max_order, distribution", "W")
	},
	{
		(CHAR*) N_GET_WD_SCORING,
		(&CSGInterface::a_get_WD_scoring),
		(CHAR*) USAGE_IO(N_GET_WD_SCORING, "max_order", "W")
	},
	{
		(CHAR*) N_GET_WD_POS_WEIGHTS,
		(&CSGInterface::a_get_WD_position_weights),
		(CHAR*) USAGE_O(N_GET_WD_POS_WEIGHTS, "W")
	},
	{
		(CHAR*) N_GET_LAST_SUBKERNEL_WEIGHTS,
		(&CSGInterface::a_get_last_subkernel_weights),
		(CHAR*) USAGE_O(N_GET_LAST_SUBKERNEL_WEIGHTS, "W")
	},
	{
		(CHAR*) N_COMPUTE_BY_SUBKERNELS,
		(&CSGInterface::a_compute_by_subkernels),
		(CHAR*) USAGE_O(N_COMPUTE_BY_SUBKERNELS, "W")
	},
	{
		(CHAR*) N_GET_KERNEL_OPTIMIZATION,
		(&CSGInterface::a_get_kernel_optimization),
		(CHAR*) USAGE_O(N_GET_KERNEL_OPTIMIZATION, "W")
	},
	{
		(CHAR*) N_PLUGIN_ESTIMATE_CLASSIFY_EXAMPLE,
		(&CSGInterface::a_plugin_estimate_classify_example),
		(CHAR*) USAGE_IO(N_PLUGIN_ESTIMATE_CLASSIFY_EXAMPLE, "feature_vector_index", "result")
	},
	{
		(CHAR*) N_PLUGIN_ESTIMATE_CLASSIFY,
		(&CSGInterface::a_plugin_estimate_classify),
		(CHAR*) USAGE_O(N_PLUGIN_ESTIMATE_CLASSIFY, "result")
	},
	{
		(CHAR*) N_SET_PLUGIN_ESTIMATE,
		(&CSGInterface::a_set_plugin_estimate),
		(CHAR*) USAGE_I(N_SET_PLUGIN_ESTIMATE, "emission_probs, model_sizes")
	},
	{
		(CHAR*) N_GET_PLUGIN_ESTIMATE,
		(&CSGInterface::a_get_plugin_estimate),
		(CHAR*) USAGE_O(N_GET_PLUGIN_ESTIMATE, "emission_probs, model_sizes")
	},
	{
		(CHAR*) N_CLASSIFY,
		(&CSGInterface::a_classify),
		(CHAR*) USAGE_O(N_CLASSIFY, "result")
	},
	{
		(CHAR*) N_CLASSIFY_EXAMPLE,
		(&CSGInterface::a_classify_example),
		(CHAR*) USAGE_IO(N_CLASSIFY_EXAMPLE, "feature_vector_index", "result")
	},
	{
		(CHAR*) N_SVM_CLASSIFY_EXAMPLE,
		(&CSGInterface::a_classify_example),
		(CHAR*) USAGE_IO(N_SVM_CLASSIFY_EXAMPLE, "feature_vector_index", "result")
	},
	{
		(CHAR*) N_GET_CLASSIFIER,
		(&CSGInterface::a_get_classifier),
		(CHAR*) USAGE_O(N_GET_CLASSIFIER, "bias, weights")
	},
	{
		(CHAR*) N_GET_SVM,
		(&CSGInterface::a_get_svm),
		(CHAR*) USAGE_O(N_GET_SVM, "bias, alphas")
	},
	{
		(CHAR*) N_SET_SVM,
		(&CSGInterface::a_set_svm),
		(CHAR*) USAGE_I(N_SET_SVM, "bias, alphas")
	},
	{
		(CHAR*) N_GET_SVM_OBJECTIVE,
		(&CSGInterface::a_get_svm_objective),
		(CHAR*) USAGE_O(N_GET_SVM_OBJECTIVE, "objective")
	},
	{
		(CHAR*) N_RELATIVE_ENTROPY,
		(&CSGInterface::a_relative_entropy),
		(CHAR*) USAGE_O(N_RELATIVE_ENTROPY, "result")
	},
	{
		(CHAR*) N_ENTROPY,
		(&CSGInterface::a_entropy),
		(CHAR*) USAGE_O(N_ENTROPY, "result")
	},
	{
		(CHAR*) N_HMM_CLASSIFY,
		(&CSGInterface::a_hmm_classify),
		(CHAR*) USAGE_O(N_HMM_CLASSIFY, "result")
	},
	{
		(CHAR*) N_ONE_CLASS_LINEAR_HMM_CLASSIFY,
		(&CSGInterface::a_one_class_linear_hmm_classify),
		(CHAR*) USAGE_O(N_ONE_CLASS_LINEAR_HMM_CLASSIFY, "result")
	},
	{
		(CHAR*) N_ONE_CLASS_HMM_CLASSIFY,
		(&CSGInterface::a_one_class_hmm_classify),
		(CHAR*) USAGE_O(N_ONE_CLASS_HMM_CLASSIFY, "result")
	},
	{
		(CHAR*) N_ONE_CLASS_HMM_CLASSIFY_EXAMPLE,
		(&CSGInterface::a_one_class_hmm_classify_example),
		(CHAR*) USAGE_IO(N_ONE_CLASS_HMM_CLASSIFY_EXAMPLE, "feature_vector_inde", "result")
	},
	{
		(CHAR*) N_HMM_CLASSIFY_EXAMPLE,
		(&CSGInterface::a_hmm_classify_example),
		(CHAR*) USAGE_IO(N_HMM_CLASSIFY_EXAMPLE, "feature_vector_index", "result")},
	{
		(CHAR*) N_HMM_LIKELIHOOD,
		(&CSGInterface::a_hmm_likelihood),
		(CHAR*) USAGE_O(N_HMM_LIKELIHOOD, "likelihood")
	},
	{
		(CHAR*) N_GET_VITERBI_PATH,
		(&CSGInterface::a_get_viterbi_path),
		(CHAR*) USAGE_IO(N_GET_VITERBI_PATH, "dim", "path, likelihood")
	},
	{
		(CHAR*) N_GET_HMM,
		(&CSGInterface::a_get_hmm),
		(CHAR*) USAGE_O(N_GET_HMM, "p, q, a, b")
	},
	{
		(CHAR*) N_APPEND_HMM,
		(&CSGInterface::a_append_hmm),
		(CHAR*) USAGE_I(N_APPEND_HMM, "p, q, a, b")
	},
	{
		(CHAR*) N_SET_HMM,
		(&CSGInterface::a_set_hmm),
		(CHAR*) USAGE_I(N_SET_HMM, "p, q, a, b")
	},
	{
		(CHAR*) N_HELP,
		(&CSGInterface::a_help),
		(CHAR*) USAGE(N_HELP)
	},
	{
		(CHAR*) "test",
		(&CSGInterface::a_test),
		(CHAR*) USAGE_I("test", "arg")
	},
	{NULL, NULL, NULL}        /* Sentinel */
};


CSGInterface::CSGInterface()
 : m_lhs_counter(0), m_rhs_counter(0), m_nlhs(0), m_nrhs(0)
{
}

CSGInterface::~CSGInterface()
{
}

////////////////////////////////////////////////////////////////////////////
// actions
////////////////////////////////////////////////////////////////////////////

bool CSGInterface::a_crc()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT slen=0;
	CHAR* string=get_string(slen);
	ASSERT(string);
	BYTE* bstring=new BYTE[slen];
	ASSERT(bstring);

	for (INT i=0; i<slen; i++)
		bstring[i]=string[i];
	delete[] string;

	INT val=CMath::crc32(bstring, slen);
	delete[] bstring;

	CSGInterfaceVector iv(&val, 1);
	set_vector(iv);

	return true;
}

bool CSGInterface::a_translate_string()
{
	if (m_nlhs!=1 || m_nrhs!=4)
		return false;

	CSGInterfaceVector iv(SGIDT_DREAL);
	get_vector(iv);
	const DREAL* string=NULL;
	UINT len;
	iv.get(string, len);

	UINT order=(UINT) get_int();
	INT start=get_int();

	const INT max_val=2; /* DNA->2bits */
	UINT i,j;

	WORD* obs=new WORD[len];
	ASSERT(obs);

	for (i=0; i<len; i++)
	{
		switch ((CHAR) string[i])
		{
			case 'A': obs[i]=0; break;
			case 'C': obs[i]=1; break;
			case 'G': obs[i]=2; break;
			case 'T': obs[i]=3; break;
			case 'a': obs[i]=0; break;
			case 'c': obs[i]=1; break;
			case 'g': obs[i]=2; break;
			case 't': obs[i]=3; break;
			default: SG_SERROR("Wrong letter in string.\n");
		}
	}

	//convert interval of size T
	for (i=len-1; i>=order-1; i--)
	{
		WORD value=0;
		for (j=i; j>=i-order+1; j--)
			value=(value>>max_val) | ((obs[j])<<(max_val*(order-1)));
		
		obs[i]=(WORD) value;
	}
	
	for (i=order-2;i>=0;i--)
	{
		WORD value=0;
		for (j=i; j>=i-order+1; j--)
		{
			value= (value >> max_val);
			if (j>=0)
				value|=(obs[j]) << (max_val * (order-1));
		}
		obs[i]=value;
	}

	DREAL* real_obs=new DREAL[len];
	ASSERT(real_obs);

	for (i=start; i<len; i++)
		real_obs[i-start]=(DREAL) obs[i];
	delete[] obs;

	iv.set(real_obs, len);
	set_vector(iv);
	delete[] real_obs;

	return true;
}

bool CSGInterface::a_best_path_2struct()
{
	if (m_nlhs!=3 || m_nrhs!=12)
		return false;

	SG_SERROR("Sorry, this parameter list is awful!\n");
	
	return true;
}

bool CSGInterface::a_best_path_trans()
{
	if (!(m_nlhs==3 && (m_nrhs==15 || m_nrhs==17)))
		return false;

	SG_SERROR("Sorry, this parameter list is awful!\n");
	
	return true;
}

bool CSGInterface::a_best_path_trans_deriv()
{
	if (!((m_nlhs==5 && m_nrhs==14) || (m_nlhs==6 && m_nrhs==16)))
		return false;

	SG_SERROR("Sorry, this parameter list is awful!\n");

	return true;
}

bool CSGInterface::a_best_path_no_b()
{
	if (m_nlhs!=2 || m_nrhs!=5)
		return false;

	DREAL* p=NULL;
	INT M_p=0;
	INT N_p=0;
	get_real_matrix(p, M_p, N_p);
	INT N=N_p;

	DREAL* q=NULL;
	INT M_q=0;
	INT N_q=0;
	get_real_matrix(q, M_q, N_q);

	DREAL* a=NULL;
	INT M_a=0;
	INT N_a=0;
	get_real_matrix(a, M_a, N_a);

	if (N_p!=N || M_p!=1 || N_q!=N || M_q!=1 || N_a!=N || M_a!=N)
	{
		delete[] p;
		delete[] q;
		delete[] a;
		SG_SERROR("Model matrices not matching in size.\n");
	}

	INT max_iter=get_int();
	if (max_iter<1)
	{
		delete[] p;
		delete[] q;
		delete[] a;
		SG_SERROR("max_iter < 1.\n");
	}

	CDynProg* h=new CDynProg();
	ASSERT(h);
	h->set_N(N);
	h->set_p_vector(p, N);
	h->set_q_vector(q, N);
	h->set_a(a, N, N);

	INT* path=new INT[max_iter];
	ASSERT(path);
	INT best_iter=0;
	DREAL prob=h->best_path_no_b(max_iter, best_iter, path);
	delete h;
	delete[] p;
	delete[] q;
	delete[] a;

	set_real_vector(&prob, 1);
	set_int_vector(path, best_iter+1);
	delete[] path;

	return true;
}

bool CSGInterface::a_best_path_trans_simple()
{
	if (m_nlhs!=2 || m_nrhs!=6)
		return false;

	DREAL* p=NULL;
	INT M_p=0;
	INT N_p=0;
	get_real_matrix(p, M_p, N_p);
	INT N=N_p;

	DREAL* q=NULL;
	INT M_q=0;
	INT N_q=0;
	get_real_matrix(q, M_q, N_q);

	DREAL* a_trans=NULL;
	INT M_a_trans=0;
	INT N_a_trans=0;
	get_real_matrix(a_trans, M_a_trans, N_a_trans);

	DREAL* seq=NULL;
	INT M_seq=0;
	INT N_seq=0;
	get_real_matrix(seq, M_seq, N_seq);

	if (N_p!=N || M_p!=1 || N_q!=N || M_q!=1 || N_a_trans!=3 || M_seq!=N)
	{
		delete[] p;
		delete[] q;
		delete[] a_trans;
		delete[] seq;
		SG_SERROR("Model matrices not matching in size.\n");
	}

	INT nbest=get_int();
	if (nbest<1)
	{
		delete[] p;
		delete[] q;
		delete[] a_trans;
		delete[] seq;
		SG_SERROR("nbest < 1.\n");
	}

	CDynProg* h=new CDynProg();
	ASSERT(h);
	h->set_N(N);
	h->set_p_vector(p, N);
	h->set_q_vector(q, N);
	h->set_a_trans_matrix(a_trans, M_a_trans, 3);

	INT* path=new INT[N_seq*nbest];
	ASSERT(path);
	memset(path, -1, N_seq*nbest*sizeof(INT));
	DREAL* prob=new DREAL[nbest];
	ASSERT(prob);

	h->best_path_trans_simple(seq, N_seq, nbest, prob, path);
	delete h;
	delete[] p;
	delete[] q;
	delete[] a_trans;
	delete[] seq;

	set_real_vector(prob, nbest);
	delete[] prob;
	set_int_matrix(path, nbest, N_seq);
	delete[] path;

	return true;
}


bool CSGInterface::a_best_path_no_b_trans()
{
	if (m_nlhs!=2 || m_nrhs!=6)
		return false;

	DREAL* p=NULL;
	INT M_p=0;
	INT N_p=0;
	get_real_matrix(p, M_p, N_p);
	INT N=N_p;

	DREAL* q=NULL;
	INT M_q=0;
	INT N_q=0;
	get_real_matrix(q, M_q, N_q);

	DREAL* a_trans=NULL;
	INT M_a_trans=0;
	INT N_a_trans=0;
	get_real_matrix(a_trans, M_a_trans, N_a_trans);

	if (N_p!=N || M_p!=1 || N_q!=N || M_q!=1 || N_a_trans!=3)
	{
		delete[] p;
		delete[] q;
		delete[] a_trans;
		SG_SERROR("Model matrices not matching in size.\n");
	}

	INT max_iter=get_int();
	if (max_iter<1)
	{
		delete[] p;
		delete[] q;
		delete[] a_trans;
		SG_SERROR("max_iter < 1.\n");
	}

	INT nbest=get_int();
	if (nbest<1)
	{
		delete[] p;
		delete[] q;
		delete[] a_trans;
		SG_SERROR("nbest < 1.\n");
	}

	CDynProg* h=new CDynProg();
	ASSERT(h);
	h->set_N(N);
	h->set_p_vector(p, N);
	h->set_q_vector(q, N);
	h->set_a_trans_matrix(a_trans, M_a_trans, 3);

	INT* path=new INT[(max_iter+1)*nbest];
	ASSERT(path);
	memset(path, -1, (max_iter+1)*nbest*sizeof(INT));
	INT max_best_iter=0;
	DREAL* prob=new DREAL[nbest];
	ASSERT(prob);

	h->best_path_no_b_trans(max_iter, max_best_iter, nbest, prob, path);
	delete h;
	delete[] p;
	delete[] q;
	delete[] a_trans;

	set_real_vector(prob, nbest);
	delete[] prob;
	set_int_matrix(path, nbest, max_best_iter+1);
	delete[] path;

	return true;
}

bool CSGInterface::a_get_version()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	DREAL* ver=(DREAL*) version.get_version_revision();
	set_real_vector(ver, 1);

	return true;
}

bool CSGInterface::a_set_labels()
{
	if (m_nlhs!=0 || m_nrhs!=3)
		return false;

	INT tlen=0;
	CHAR* target=get_string(tlen);
	if (!strmatch(target, tlen, "TRAIN") && !strmatch(target, tlen, "TEST"))
	{
		delete target;
		SG_SERROR("Unknown target, neither TRAIN nor TEST.\n");
	}

	INT len=0;
	DREAL* lab=NULL;
	get_real_vector(lab, len);

	CLabels* labels=new CLabels(len);
	SG_INFO("num labels: %d\n", labels->get_num_labels());

	for (INT i=0; i<len; i++)
	{
		if (!labels->set_label(i, lab[i]))
			SG_SERROR("Couldn't set label %d (of %d): %f.\n", i, len, lab[i]);
	}

	if (strmatch(target, tlen, "TRAIN"))
		gui->guilabels.set_train_labels(labels);
	else
		gui->guilabels.set_test_labels(labels);
	delete[] target;

	return true;
}

bool CSGInterface::a_get_labels()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT tlen=0;
	CHAR* target=get_string(tlen);
	CLabels* labels=NULL;

	if (strmatch(target, tlen, "TRAIN"))
		labels=gui->guilabels.get_train_labels();
	else if (strmatch(target, tlen, "TEST"))
		labels=gui->guilabels.get_test_labels();
	else
	{
		delete[] target;
		SG_SERROR("Unknown target, neither TRAIN nor TEST.\n");
	}
	delete[] target;

	if (!labels)
		SG_SERROR("No labels.\n");

	INT num_labels=labels->get_num_labels();
	DREAL* lab=new DREAL[num_labels];

	for (INT i=0; i<num_labels ; i++)
		lab[i]=labels->get_label(i);

	set_real_vector(lab, num_labels);
	delete[] lab;

	return true;
}

bool CSGInterface::a_obtain_from_position_list()
{
	if (m_nlhs!=0 || (m_nrhs!=4 && m_nrhs!=5))
		return false;

	INT tlen=0;
	CHAR* target=get_string(tlen);
	if (!strmatch(target, tlen, "TRAIN") && !strmatch(target, tlen, "TEST"))
	{
		delete[] target;
		SG_SERROR("Unknown target, neither TRAIN nor TEST.\n");
	}

	INT winsize=get_int();
	INT* shifts=NULL;
	INT num_shift=0;
	get_int_vector(shifts, num_shift);

	INT skip=0;
	if (m_nrhs==5)
		skip=get_int();

	SG_DEBUG("winsize: %d num_shifts: %d skip: %d\n", winsize, num_shift, skip);

	CDynamicArray<INT> positions(num_shift+1);

	for (INT i=0; i<num_shift; i++)
		positions.set_element(shifts[i], i);

	CFeatures* features=NULL;
	if (strmatch(target, tlen, "TRAIN"))
	{
		gui->guifeatures.invalidate_train();
		features=gui->guifeatures.get_train_features();
	}
	else
	{
		gui->guifeatures.invalidate_test();
		features=gui->guifeatures.get_test_features();
	}
	delete[] target;

	if (!features)
	{
		delete[] shifts;
		SG_SERROR("No features.\n");
	}

	if (features->get_feature_class()==C_COMBINED)
	{
		features=((CCombinedFeatures*) features)->get_last_feature_obj();
		if (!features)
		{
			delete[] shifts;
			SG_SERROR("No features from combined.\n");
		}
	}

	if (features->get_feature_class()!=C_STRING)
	{
		delete[] shifts;
		SG_SERROR("No string features.\n");
	}

	bool success=false;
	switch (features->get_feature_type())
	{
		case F_CHAR:
		{
			success=(((CStringFeatures<CHAR>*) features)->
				obtain_by_position_list(winsize, &positions, skip)>0);
			break;
		}
		case F_BYTE:
		{
			success=(((CStringFeatures<BYTE>*) features)->
				obtain_by_position_list(winsize, &positions, skip)>0);
			break;
		}
		case F_WORD:
		{
			success=(((CStringFeatures<WORD>*) features)->
				obtain_by_position_list(winsize, &positions, skip)>0);
			break;
		}
		case F_ULONG:
		{
			success=(((CStringFeatures<ULONG>*) features)->
				obtain_by_position_list(winsize, &positions, skip)>0);
			break;
		}
		default:
		{
			delete[] shifts;
			SG_SERROR("Unsupported string features type.\n");
		}
	}

	delete[] shifts;

	return success;
}

bool CSGInterface::a_get_features()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT tlen=0;
	CHAR* target=get_string(tlen);
	CFeatures* feat=NULL;

	if (strmatch(target, tlen, "TRAIN"))
		feat=gui->guifeatures.get_train_features();
	else if (strmatch(target, tlen, "TEST"))
		feat=gui->guifeatures.get_test_features();
	else
	{
		delete[] target;
		SG_SERROR("Unknown target, neither TRAIN nor TEST.\n");
	}
	delete[] target;

	switch (feat->get_feature_class())
	{
		case C_SIMPLE:
			switch (feat->get_feature_type())
			{
				case F_DREAL:
				{
					CRealFeatures* realfeat=(CRealFeatures*) feat;
					INT num_feat=realfeat->get_num_features();
					INT num_vec=realfeat->get_num_vectors();
					DREAL* result=new DREAL[num_feat*num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT num_vfeat=0;
						bool free_vec=true;
						DREAL* vec=realfeat->get_feature_vector(
							i, num_vfeat, free_vec);
						ASSERT(num_vfeat==num_feat);

						for (INT j=0; j<num_vfeat; j++)
							result[num_feat*i+j]=vec[j];
						realfeat->free_feature_vector(vec, i, free_vec);
					}

					set_real_matrix(result, num_feat, num_vec);
					delete[] result;

					break;
				}

				case F_WORD:
				{
					CWordFeatures* wordfeat=(CWordFeatures*) feat;
					INT num_feat=wordfeat->get_num_features();
					INT num_vec=wordfeat->get_num_vectors();
					WORD* result=new WORD[num_feat*num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT num_vfeat=0;
						bool free_vec=true;
						WORD* vec=wordfeat->get_feature_vector(i, num_vfeat, free_vec);
						ASSERT(num_vfeat==num_feat);

						for (INT j=0; j<num_vfeat; j++)
							result[num_feat*i+j]=vec[j];
						wordfeat->free_feature_vector(vec, i, free_vec);
					}

					set_word_matrix(result, num_feat, num_vec);
					delete[] result;

					break;
				}
				
				case F_SHORT:
				{
					CShortFeatures* shortfeat=(CShortFeatures*) feat;
					INT num_feat=shortfeat->get_num_features();
					INT num_vec=shortfeat->get_num_vectors();
					SHORT* result=new SHORT[num_feat*num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT num_vfeat=0;
						bool free_vec=true;
						SHORT* vec=shortfeat->get_feature_vector(i, num_vfeat, free_vec);
						ASSERT(num_vfeat==num_feat);

						for (INT j=0; j<num_vfeat; j++)
							result[num_feat*i+j]=vec[j];
						shortfeat->free_feature_vector(vec, i, free_vec);
					}

					set_short_matrix(result, num_feat, num_vec);
					delete[] result;

					break;
				}
				
				case F_CHAR:
				{
					CCharFeatures* charfeat=(CCharFeatures*) feat;
					INT num_feat=charfeat->get_num_features();
					INT num_vec=charfeat->get_num_vectors();
					CHAR* result=new CHAR[num_feat*num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT num_vfeat=0;
						bool free_vec=true;
						CHAR* vec=charfeat->get_feature_vector(i, num_vfeat, free_vec);
						ASSERT(num_vfeat==num_feat);

						for (INT j=0; j<num_vfeat; j++)
							result[num_feat*i+j]=vec[j];
						charfeat->free_feature_vector(vec, i, free_vec);
					}

					set_char_matrix(result, num_feat, num_vec);
					delete[] result;

					break;
				}
				
				case F_BYTE:
				{
					CByteFeatures* bytefeat=(CByteFeatures*) feat;
					INT num_feat=bytefeat->get_num_features();
					INT num_vec=bytefeat->get_num_vectors();
					BYTE* result=new BYTE[num_feat*num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT num_vfeat=0;
						bool free_vec=true;
						BYTE* vec=bytefeat->get_feature_vector(i, num_vfeat, free_vec);
						ASSERT(num_vfeat==num_feat);

						for (INT j=0; j<num_vfeat; j++)
							result[num_feat*i+j]=vec[j];
						bytefeat->free_feature_vector(vec, i, free_vec);
					}

					set_byte_matrix(result, num_feat, num_vec);
					delete[] result;

					break;
				}
				
				default:
					SG_SERROR("%s not implemented.\n", feat->get_feature_type());
			}
		break;

		case C_SPARSE:
			switch (feat->get_feature_type())
			{
				case F_DREAL:
				{
					LONG nnz=((CSparseFeatures<DREAL>*) feat)->
						get_num_nonzero_entries();
					INT num_vec=feat->get_num_vectors();
					INT num_feat=
						((CSparseFeatures<DREAL>*) feat)->get_num_features();

					SG_DEBUG("sparse matrix has %d rows, %d cols and %d nnz elemements\n", num_feat, num_vec, nnz);

					TSparse<DREAL>* result=new TSparse<DREAL>[num_vec];
					ASSERT(result);

					for (INT i=0; i<num_vec; i++)
					{
						INT len=0;
						bool dofree=false;
						TSparseEntry<DREAL>* vec=
							((CSparseFeatures<DREAL>*) feat)->
								get_sparse_feature_vector(i, len, dofree);
						result[i].features=new TSparseEntry<DREAL>[len];

						for (INT j=0; j<len; j++)
						{
							result[i].features[j].entry=vec[j].entry;
							result[i].features[j].feat_index=vec[j].feat_index;
						}
						((CSparseFeatures<DREAL>*) feat)->
							free_feature_vector(vec, len, dofree);
					}

					set_real_sparsematrix(result, num_feat, num_vec);
					delete[] result;
				}
				break;

				default:
					SG_SERROR("not implemented\n");
			}
		break;

		case C_STRING:
			switch (feat->get_feature_type())
			{
				case F_CHAR:
				{
					INT num_vec=feat->get_num_vectors();
					T_STRING<CHAR>* list=new T_STRING<CHAR>[num_vec];
					for (INT i=0; i<num_vec; i++)
					{
						INT len=0;
						CHAR* vec=((CStringFeatures<CHAR>*) feat)->
							get_feature_vector(i, len);

						if (len>0)
							list[i].string=vec;
						else
							list[i].string=NULL;
					}

					set_string_list(list, num_vec);
					delete[] list;
				}
				break;

				case F_WORD:
				{
					INT num_vec=feat->get_num_vectors();
					T_STRING<WORD>* list=new T_STRING<WORD>[num_vec];
					for (INT i=0; i<num_vec; i++)
					{
						INT len=0;
						WORD* vec=((CStringFeatures<WORD>*) feat)->
							get_feature_vector(i, len);

						if (len>0)
							list[i].string=vec;
						else
							list[i].string=NULL;
					}

					set_string_list(list, num_vec);
					delete[] list;
				}
				break;

				default:
					SG_SERROR("not implemented\n");
			}
		break;

		default:
			SG_SERROR( "not implemented\n");
	}

	return true;
}

bool CSGInterface::a_add_features()
{
	if (m_nlhs!=0 || (m_nrhs!=3 && m_nrhs!=4))
		return false;

	return do_set_features(true);
}

bool CSGInterface::a_set_features()
{
	if (m_nlhs!=0 || (m_nrhs!=3 && m_nrhs!=4))
		return false;

	return do_set_features(false);
}

bool CSGInterface::do_set_features(bool add)
{
	INT tlen=0;
	CHAR* target=get_string(tlen);
	if (!strmatch(target, tlen, "TRAIN") && !strmatch(target, tlen, "TEST"))
	{
		delete[] target;
		SG_SERROR("Unknown target, neither TRAIN nor TEST.\n");
	}

	CFeatures* feat=NULL;
/*
	INT num_feat=0;
	INT num_vec=0;

	if (is_sparse() && is_numeric())
		DREAL* fmatrix=NULL;
		get_real_sparsematrix(fmatrix, num_feat, num_vec);

		feat=new CSparseFeatures<DREAL)(0);
		ASSERT(feat);
		((CSparseFeatures<DREAL>*) feat)->
			set_sparse_feature_matrix(fmatrix, num_feat, num_vec);
	}
	else if (is_double())
	{
		DREAL* fmatrix=NULL;
		get_real_matrix(fmatrix, num_feat, num_vec);

		feat=new CRealFeatures(0);
		ASSERT(feat);
		((CRealFeatures*) feat)->
			set_feature_matrix(fmatrix, num_feat, num_vec);
	}
	else if (is_char())
	{
		if (m_nrhs!=4)
			SG_SERROR("Please specify alphabet / type!\n");

		INT alen=0;
		//FIXME:  problem -> arg4 defines arg3!!!
		CHAR* alphabet_str=get_string(alen, 4);
		
		if (strmatch(alphabet_str, alen, "DNABINFILE"))
		{
			CHAR* fname=get_string();
			ASSERT(fname);

			feat=new CStringFeatures<BYTE>(DNA);
			ASSERT(feat);
			if (!((CStringFeatures<BYTE>*) feat)->load_dna_file(fname))
			{
				delete alphabet_str;
				delete fname;
				delete feat;
				SG_SERROR("Couldn't load DNA features from file.\n");
			}

			delete fname;
		}
		else
		{
			CAlphabet* alphabet=new CAlphabet(alphabet_str, alen);
			ASSERT(alphabet);
			T_STRING<CHAR>* fmatrix=NULL;
			get_string_list(fmatrix, num_feat, num_vec);

			INT maxlen=0;
			for (INT i=0; i<num_vec; i++)
				maxlen=CMath::max(maxlen, fmatrix[i].length);

			feat=new CStringFeatures<CHAR>(alphabet);
			ASSERT(feat);
			if (!((CStringFeatures<CHAR>*) feat)->
				set_features(fmatrix, num_vec, maxlen))
			{
				delete alphabet_str;
				delete alphabet;
				delete feat;
				SG_SERROR("Couldn't set string features.\n");
			}

			delete alphabet;
		}

		delete alphabet_str;
	}
	else if (is_byte())
	{
		if (m_nrhs!=4)
			SG_SERROR("Please specify alphabet!\n");

		INT alen=0;
		CHAR* alphabet_str=get_string(alen);
		ASSERT(alphabet_str);
		CAlphabet* alphabet=new CAlphabet(alphabet_str, alen);
		ASSERT(alphabet);
		delete alphabet_str;

		T_STRING<BYTE>* fmatrix=NULL;
		get_string_list(fmatrix, num_feat, num_vec);

		INT maxlen=0;
		for (INT i=0; i<num_vec; i++)
			maxlen=CMath::max(maxlen, fmatrix[i].length);

		feat=new CStringFeatures<BYTE>(alphabet);
		ASSERT(feat);
		if (!((CStringFeatures<BYTE>*) feat)->set_features(fmatrix, num_vec, maxlen))
		{
			delete alphabet;
			delete feat;
			SG_SERROR("Couldnt set byte string features.\n");
			f=NULL;
		}
	}

/// eh?
			else if (mxIsCell(mx_feat))
			{
				int num_vec=mxGetNumberOfElements(mx_feat);

				ASSERT(num_vec>=1 && mxGetCell(mx_feat, 0));


				if (mxIsChar(mxGetCell(mx_feat, 0)))
				{
					if (nrhs==4)
					{
						INT len=0;
						CHAR* al = CGUIMatlab::get_mxString(vals[3], len);
						CAlphabet* alpha = new CAlphabet(al, len);
						T_STRING<CHAR>* sc=new T_STRING<CHAR>[num_vec];
						ASSERT(alpha);
						ASSERT(sc);

						int maxlen=0;

						for (int i=0; i<num_vec; i++)
						{
							mxArray* e=mxGetCell(mx_feat, i);
							ASSERT(e && mxIsChar(e));
							sc[i].string=get_mxString(e, len);

							if (sc[i].string)
							{
								sc[i].length=len;
								maxlen=CMath::max(maxlen, sc[i].length);
							}
							else
							{
								SG_WARNING( "string with index %d has zero length\n", i+1);
								sc[i].length=0;
							}
						}

						f= new CStringFeatures<CHAR>(alpha);
						ASSERT(f);

						if (!((CStringFeatures<CHAR>*) f)->set_features(sc, num_vec, maxlen))
						{
							delete f;
							f=NULL;
						}
					}
					else
						SG_SERROR( "please specify alphabet!\n");
				}
				else if (mxIsClass(mxGetCell(mx_feat, 0), "uint8") || mxIsClass(mxGetCell(mx_feat, 0), "int8"))
				{
					if (nrhs==4)
					{
						INT len=0;
						CHAR* al = CGUIMatlab::get_mxString(vals[3], len);
						CAlphabet* alpha = new CAlphabet(al, len);
						T_STRING<BYTE>* sc=new T_STRING<BYTE>[num_vec];
						ASSERT(alpha);

						int maxlen=0;

						for (int i=0; i<num_vec; i++)
						{
							mxArray* e=mxGetCell(mx_feat, i);
							ASSERT(e && (mxIsClass(e, "uint8") || mxIsClass(e, "int8")));
							INT _len=0;
							sc[i].string=get_mxBytes(e, _len);
							if (sc[i].string)
							{
								sc[i].length=_len;
								maxlen=CMath::max(maxlen, sc[i].length);
							}
							else
							{
								SG_WARNING( "string with index %d has zero length\n", i+1);
								sc[i].length=0;
							}
						}

						f= new CStringFeatures<BYTE>(alpha);
						ASSERT(f);

						if (!((CStringFeatures<BYTE>*) f)->set_features(sc, num_vec, maxlen))
						{
							delete f;
							f=NULL;
						}
					}
					else
						SG_SERROR( "please specify alphabet!\n");
				}

			}
*/

	if (strmatch(target, tlen, "TRAIN"))
	{
		if (add)
			gui->guifeatures.add_train_features(feat);
		else
			gui->guifeatures.set_train_features(feat);
	}
	else
	{
		if (add)
			gui->guifeatures.add_test_features(feat);
		else
			gui->guifeatures.set_test_features(feat);
	}

	delete[] target;

	return true;
}

bool CSGInterface::a_get_distance_matrix()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CDistance* distance=gui->guidistance.get_distance();
	if (!distance || !distance->get_rhs() || !distance->get_lhs())
		SG_SERROR("No distance defined.\n");

	INT num_vec1=distance->get_lhs()->get_num_vectors();
	INT num_vec2=distance->get_rhs()->get_num_vectors();
	DREAL* dmatrix=NULL;
	distance->get_distance_matrix_real(num_vec1, num_vec2, dmatrix);

	set_real_matrix(dmatrix, num_vec1, num_vec2);
	delete[] dmatrix;

	return true;
}

bool CSGInterface::a_get_kernel_matrix()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel || !kernel->get_rhs() || !kernel->get_lhs())
		SG_SERROR("No kernel defined.\n");

	INT num_vec1=kernel->get_lhs()->get_num_vectors();
	INT num_vec2=kernel->get_rhs()->get_num_vectors();
	DREAL* kmatrix=NULL;
	kernel->get_kernel_matrix_real(num_vec1, num_vec2, kmatrix);

	set_real_matrix(kmatrix, num_vec1, num_vec2);
	delete[] kmatrix;

	return true;
}

bool CSGInterface::a_set_custom_kernel()
{
	if (m_nlhs!=0 || m_nrhs!=3)
		return false;

	CCustomKernel* kernel=(CCustomKernel*) gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel defined.\n");

	if (kernel->get_kernel_type()==K_COMBINED)
	{
		SG_DEBUG("Identified combined kernel.\n");
		kernel=(CCustomKernel*) ((CCombinedKernel*) kernel)->
			get_last_kernel();
		if (!kernel)
			SG_SERROR("No last kernel defined.\n");
	}

	if (kernel->get_kernel_type()!=K_CUSTOM)
		SG_SERROR("Not a custom kernel.\n");

	DREAL* kmatrix=NULL;
	INT num_feat=0;
	INT num_vec=0;
	get_real_matrix(kmatrix, num_feat, num_vec);

	INT tlen=0;
	CHAR* type=get_string(tlen);

	if (!strmatch(type, tlen, "DIAG") && !strmatch(type, tlen, "FULL"))
	{
		delete[] kmatrix;
		delete[] type;
		SG_SERROR("Undefined type, not DIAG, FULL or FULL2DIAG.\n");
	}

	bool source_is_diag=false;
	bool dest_is_diag=false;
	if (strmatch(type, tlen, "FULL2DIAG"))
		dest_is_diag=true;
	else if (strmatch(type, tlen, "DIAG"))
	{
		source_is_diag=true;
		dest_is_diag=true;
	}
	// change nothing if FULL

	bool success=false;
	if (source_is_diag && dest_is_diag && num_vec==num_feat)
		success=kernel->set_triangle_kernel_matrix_from_triangle(
			kmatrix, num_vec);
	else if (!source_is_diag && dest_is_diag && num_vec==num_feat)
		success=kernel->set_triangle_kernel_matrix_from_full(
			kmatrix, num_feat, num_vec);
	else
		success=kernel->set_full_kernel_matrix_from_full(
			kmatrix, num_feat, num_vec);

	delete[] kmatrix;

	return success;
}

bool CSGInterface::a_set_WD_position_weights()
{
	if (m_nlhs!=0 || m_nrhs<2 || m_nrhs>3)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_COMBINED)
		SG_SERROR("Only works for combined kernels.\n");

	kernel=((CCombinedKernel*) kernel)->get_last_kernel();
	if (!kernel)
		SG_SERROR("No last kernel.\n");

	EKernelType ktype=kernel->get_kernel_type();
	if (ktype!=K_WEIGHTEDDEGREE && ktype!=K_WEIGHTEDDEGREEPOS)
		SG_SERROR("Unsupported kernel.\n");

	bool success=false;
	DREAL* weights=NULL;
	INT dim=0;
	INT len=0;
	get_real_matrix(weights, dim, len);

	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=
			(CWeightedDegreeStringKernel*) kernel;

		if (dim!=1 & len>0)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be 1 x seq_length or 0x0\n");
		}

		success=k->set_position_weights(weights, len);
	}
	else
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		CHAR* target=NULL;
		bool is_train=true;

		if (m_nrhs==3)
		{
			INT tlen=0;
			target=get_string(tlen);
			if (!target)
			{
				delete[] weights;
				SG_SERROR("Couldn't find second argument to method.\n");
			}

			if (!strmatch(target, tlen, "TRAIN") && !strmatch(target, tlen, "TEST"))
			{
				delete[] weights;
				delete[] target;
				SG_SERROR("Second argument none of TRAIN or TEST.\n");
			}

			if (strmatch(target, tlen, "TEST"))
				is_train=false;
		}

		if (dim!=1 & len>0)
		{
			delete[] weights;
			delete[] target;
			SG_SERROR("Dimension mismatch (should be 1 x seq_length or 0x0\n");
		}

		if (dim==0 & len==0)
		{
			if (m_nlhs==3)
			{
				if (is_train)
					success=k->delete_position_weights_lhs();
				else
					success=k->delete_position_weights_rhs();
			}
			else
				success=k->delete_position_weights();
		}
		else
		{
			if (m_nlhs==3)
			{
				if (is_train)
					success=k->set_position_weights_lhs(weights, dim, len);
				else
					success=k->set_position_weights_rhs(weights, dim, len);
			}
			else
				success=k->set_position_weights(weights, len);
		}

		delete[] target;
	}

	delete[] weights;
	return success;
}

bool CSGInterface::a_set_subkernel_weights()
{
	if (m_nlhs!=0 || m_nrhs!=2)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");

	bool success=false;
	DREAL* weights=NULL;
	INT dim=0;
	INT len=0;
	get_real_matrix(weights, dim, len);

	EKernelType ktype=kernel->get_kernel_type();
	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=
			(CWeightedDegreeStringKernel*) kernel;
		INT degree=k->get_degree();
		if (dim!=degree || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else if (ktype==K_WEIGHTEDDEGREEPOS)
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		INT degree=k->get_degree();
		if (dim!=degree || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else // all other kernels
	{
		INT num_subkernels=kernel->get_num_subkernels();
		if (dim!=1 || len!=num_subkernels)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be 1 x num_subkernels)\n");
		}

		kernel->set_subkernel_weights(weights, len);
		success=true;
	}

	delete[] weights;
	return success;
}

bool CSGInterface::a_set_subkernel_weights_combined()
{
	if (m_nlhs!=0 || m_nrhs!=3)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_COMBINED)
		SG_SERROR("Only works for combined kernels.\n");

	bool success=false;
	DREAL* weights=NULL;
	INT dim=0;
	INT len=0;
	get_real_matrix(weights, dim, len);

	INT idx=get_int();
	SG_DEBUG("using kernel_idx=%i\n", idx);

	kernel=((CCombinedKernel*) kernel)->get_kernel(idx);
	if (!kernel)
	{
		delete[] weights;
		SG_SERROR("No subkernel at idx %d.\n", idx);
	}

	EKernelType ktype=kernel->get_kernel_type();
	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=
			(CWeightedDegreeStringKernel*) kernel;
		INT degree=k->get_degree();
		if (dim!=degree || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else if (ktype==K_WEIGHTEDDEGREEPOS)
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		INT degree=k->get_degree();
		if (dim!=degree || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else // all other kernels
	{
		INT num_subkernels=kernel->get_num_subkernels();
		if (dim!=1 || len!=num_subkernels)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be 1 x num_subkernels)\n");
		}

		kernel->set_subkernel_weights(weights, len);
		success=true;
	}

	delete[] weights;
	return success;
}

bool CSGInterface::a_set_last_subkernel_weights()
{
	if (m_nlhs!=0 || m_nrhs!=2)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_COMBINED)
		SG_SERROR("Only works for Combined kernels.\n");

	kernel=((CCombinedKernel*) kernel)->get_last_kernel();
	if (!kernel)
		SG_SERROR("No last kernel.\n");

	bool success=false;
	DREAL* weights=NULL;
	INT dim=0;
	INT len=0;
	get_real_matrix(weights, dim, len);

	EKernelType ktype=kernel->get_kernel_type();
	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=(CWeightedDegreeStringKernel*) kernel;
		if (dim!=k->get_degree() || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else if (ktype==K_WEIGHTEDDEGREEPOS)
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		if (dim!=k->get_degree() || len<1)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be de(seq_length | 1) x degree)\n");
		}

		if (len==1)
			len=0;

		success=k->set_weights(weights, dim, len);
	}
	else // all other kernels
	{
		INT num_subkernels=kernel->get_num_subkernels();
		if (dim!=1 || len!=num_subkernels)
		{
			delete[] weights;
			SG_SERROR("Dimension mismatch (should be 1 x num_subkernels)\n");
		}

		kernel->set_subkernel_weights(weights, len);
		success=true;
	}

	delete[] weights;
	return success;
}

bool CSGInterface::a_get_SPEC_consensus()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_COMMWORDSTRING)
		SG_SERROR("Only works for CommWordString kernels.\n");

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	ASSERT(svm);
	INT num_suppvec=svm->get_num_support_vectors();
	INT* sv_idx=new INT[num_suppvec];
	DREAL* sv_weight=new DREAL[num_suppvec];
	INT num_feat=0;

	for (INT i=0; i<num_suppvec; i++)
	{
		sv_idx[i]=svm->get_support_vector(i);
		sv_weight[i]=svm->get_alpha(i);
	}

	CHAR* consensus=((CCommWordStringKernel*) kernel)->compute_consensus(
		num_feat, num_suppvec, sv_idx, sv_weight);
	delete[] sv_idx;
	delete[] sv_weight;

	set_char_vector(consensus, num_feat);
	delete[] consensus;

	return true;
}

bool CSGInterface::a_get_SPEC_scoring()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT max_order=get_int();
	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");

	EKernelType ktype=kernel->get_kernel_type();
	if (ktype!=K_COMMWORDSTRING && ktype!=K_WEIGHTEDCOMMWORDSTRING)
		SG_SERROR("Only works for (Weighted) CommWordString kernels.\n");

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	ASSERT(svm);
	INT num_suppvec=svm->get_num_support_vectors();
	INT* sv_idx=new INT[num_suppvec];
	DREAL* sv_weight=new DREAL[num_suppvec];
	INT num_feat=0;
	INT num_sym=0;

	for (INT i=0; i<num_suppvec; i++)
	{
		sv_idx[i]=svm->get_support_vector(i);
		sv_weight[i]=svm->get_alpha(i);
	}

	if ((max_order<1) || (max_order>8))
	{
		SG_WARNING( "max_order out of range 1..8 (%d). setting to 1\n", max_order);
		max_order=1;
	}

	DREAL* position_weights=NULL;
	if (ktype==K_COMMWORDSTRING)
		position_weights=((CCommWordStringKernel*) kernel)->compute_scoring(
			max_order, num_feat, num_sym, NULL,
			num_suppvec, sv_idx, sv_weight);
	else
		position_weights=((CWeightedCommWordStringKernel*) kernel)->compute_scoring(
			max_order, num_feat, num_sym, NULL,
			num_suppvec, sv_idx, sv_weight);
	delete[] sv_idx;
	delete[] sv_weight;

	set_real_matrix(position_weights, num_sym, num_feat);
	delete[] position_weights;

	return true;
}

bool CSGInterface::a_get_WD_consensus()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_WEIGHTEDDEGREEPOS)
		SG_SERROR("Only works for Weighted Degree Position kernels.\n");

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	ASSERT(svm);
	INT num_suppvec=svm->get_num_support_vectors();
	INT* sv_idx=new INT[num_suppvec];
	DREAL* sv_weight=new DREAL[num_suppvec];
	INT num_feat=0;

	for (INT i=0; i<num_suppvec; i++)
	{
		sv_idx[i]=svm->get_support_vector(i);
		sv_weight[i]=svm->get_alpha(i);
	}

	CHAR* consensus=((CWeightedDegreePositionStringKernel*) kernel)->compute_consensus(
			num_feat, num_suppvec, sv_idx, sv_weight);
	delete[] sv_idx;
	delete[] sv_weight;

	set_char_vector(consensus, num_feat);
	delete[] consensus;

	return true;
}

bool CSGInterface::a_compute_POIM_WD()
{
	if (m_nlhs!=1 || m_nrhs!=3)
		return false;

	INT max_order=get_int();
	DREAL* distribution=NULL;
	INT num_dfeat=0;
	INT num_dvec=0;

	get_real_matrix(distribution, num_dfeat, num_dvec);
	if (!distribution)
		SG_SERROR("Wrong distribution.\n");

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
	{
		delete[] distribution;
		SG_SERROR("No Kernel.\n");
	}
	if (kernel->get_kernel_type()!=K_WEIGHTEDDEGREEPOS)
	{
		delete[] distribution;
		SG_SERROR("Only works for Weighted Degree Position kernels.\n");
	}

	INT seqlen=0;
	INT num_sym=0;
	CStringFeatures<CHAR>* sfeat=(CStringFeatures<CHAR>*)
		(((CWeightedDegreePositionStringKernel*) kernel)->get_lhs());
	ASSERT(sfeat);
	seqlen=sfeat->get_max_vector_length();
	num_sym=(INT) sfeat->get_num_symbols();

	if (num_dvec!=seqlen || num_dfeat!=num_sym)
	{
		delete[] distribution;
		SG_SERROR("distribution should have (seqlen x num_sym) elements"
				"(seqlen: %d vs. %d symbols: %d vs. %d)\n", seqlen,
				num_dvec, num_sym, num_dfeat);
	}

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	ASSERT(svm);
	INT num_suppvec=svm->get_num_support_vectors();
	INT* sv_idx=new INT[num_suppvec];
	ASSERT(sv_idx);
	DREAL* sv_weight=new DREAL[num_suppvec];
	ASSERT(sv_weight);

	for (INT i=0; i<num_suppvec; i++)
	{
		sv_idx[i]=svm->get_support_vector(i);
		sv_weight[i]=svm->get_alpha(i);
	}

	/*
	if ((max_order < 1) || (max_order > 12))
	{
		SG_WARNING( "max_order out of range 1..12 (%d). setting to 1.\n", max_order);
		max_order=1;
	}
	*/

	DREAL* position_weights;
	position_weights=((CWeightedDegreePositionStringKernel*) kernel)->compute_POIM(
			max_order, seqlen, num_sym, NULL,
			num_suppvec, sv_idx, sv_weight, distribution);
	delete[] sv_idx;
	delete[] sv_weight;
	delete[] distribution;

	set_real_matrix(position_weights, num_sym, seqlen);
	delete[] position_weights;

	return true;
}

bool CSGInterface::a_get_WD_scoring()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT max_order=get_int();

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_WEIGHTEDDEGREEPOS)
		SG_SERROR("Only works for Weighted Degree Position kernels.\n");

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	ASSERT(svm);
	INT num_suppvec=svm->get_num_support_vectors();
	INT* sv_idx=new INT[num_suppvec];
	DREAL* sv_weight=new DREAL[num_suppvec];
	INT num_feat=0;
	INT num_sym=0;

	for (INT i=0; i<num_suppvec; i++)
	{
		sv_idx[i]=svm->get_support_vector(i);
		sv_weight[i]=svm->get_alpha(i);
	}

	if ((max_order<1) || (max_order>12))
	{
		SG_WARNING("max_order out of range 1..12 (%d). setting to 1\n", max_order);
		max_order=1;
	}

	DREAL* position_weights=
		((CWeightedDegreePositionStringKernel*) kernel)->compute_scoring(
			max_order, num_feat, num_sym, NULL, num_suppvec, sv_idx, sv_weight);
	delete[] sv_idx;
	delete[] sv_weight;

	set_real_matrix(position_weights, num_sym, num_feat);
	delete[] position_weights;

	return true;
}

bool CSGInterface::a_get_WD_position_weights()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (kernel->get_kernel_type()!=K_COMBINED)
		SG_SERROR("Only works for Combined kernels.\n");

	kernel=((CCombinedKernel*) kernel)->get_last_kernel();
	if (!kernel)
		SG_SERROR("Couldn't find last kernel.\n");

	if (kernel->get_kernel_type()!=K_WEIGHTEDDEGREE &&
		kernel->get_kernel_type()!=K_WEIGHTEDDEGREEPOS)
		SG_SERROR("Wrong subkernel type.\n");

	INT len=0;
	const DREAL* position_weights;

	if (kernel->get_kernel_type()==K_WEIGHTEDDEGREE)
		position_weights=((CWeightedDegreeStringKernel*) kernel)->get_position_weights(len);
	else
		position_weights=((CWeightedDegreePositionStringKernel*) kernel)->get_position_weights(len);

	if (position_weights==NULL)
		set_real_vector(position_weights, 0);
	else
		set_real_vector(position_weights, len);

	return true;
}

bool CSGInterface::a_get_last_subkernel_weights()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	EKernelType ktype=kernel->get_kernel_type();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (ktype!=K_COMBINED)
		SG_SERROR("Only works for Combined kernels.\n");

	kernel=((CCombinedKernel*) kernel)->get_last_kernel();
	if (!kernel)
		SG_SERROR("Couldn't find last kernel.\n");

	INT degree=0;
	INT len=0;

	if (ktype==K_COMBINED)
	{
		INT num_weights=0;
		const DREAL* weights=
			((CCombinedKernel*) kernel)->get_subkernel_weights(num_weights);
		set_real_vector(weights, num_weights);
		return true;
	}

	const DREAL* weights=NULL;
	if (ktype==K_WEIGHTEDDEGREE)
		weights=((CWeightedDegreeStringKernel*) kernel)->
			get_degree_weights(degree, len);
	else if (ktype==K_WEIGHTEDDEGREEPOS)
		weights=((CWeightedDegreePositionStringKernel*) kernel)->
			get_degree_weights(degree, len);
	else
		SG_SERROR("Only works for Weighted Degree (Position) kernels.\n");

	if (len==0)
		len=1;

	set_real_matrix(weights, degree, len);

	return true;
}

bool CSGInterface::a_compute_by_subkernels()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel.\n");
	if (!kernel->get_rhs())
		SG_SERROR("No rhs.\n");

	INT num_vec=kernel->get_rhs()->get_num_vectors();
	INT degree=0;
	INT len=0;
	EKernelType ktype=kernel->get_kernel_type();

	// it would be nice to have a common base class for the WD kernels
	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=(CWeightedDegreeStringKernel*) kernel;
		k->get_degree_weights(degree, len);
		if (!k->is_tree_initialized())
			SG_SERROR("Kernel optimization not initialized.\n");
	}
	else if (ktype==K_WEIGHTEDDEGREEPOS)
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		k->get_degree_weights(degree, len);
		if (!k->is_tree_initialized())
			SG_SERROR("Kernel optimization not initialized.\n");
	}
	else
		SG_SERROR("Only works for Weighted Degree (Position) kernels.\n");

	if (len==0)
		len=1;

	INT num_feat=degree*len;
	INT num=num_feat*num_vec;
	DREAL* result=new DREAL[num];
	ASSERT(result);

	for (INT i=0; i<num; i++)
		result[i]=0;

	if (ktype==K_WEIGHTEDDEGREE)
	{
		CWeightedDegreeStringKernel* k=(CWeightedDegreeStringKernel*) kernel;
		for (INT i=0; i<num_vec; i++)
			k->compute_by_tree(i, &result[i*num_feat]);
	}
	else
	{
		CWeightedDegreePositionStringKernel* k=
			(CWeightedDegreePositionStringKernel*) kernel;
		for (INT i=0; i<num_vec; i++)
			k->compute_by_tree(i, &result[i*num_feat]);
	}

	set_real_matrix(result, num_feat, num_vec);
	delete[] result;

	return true;
}

bool CSGInterface::a_get_kernel_optimization()
{
	if (m_nlhs!=1 || m_nrhs<1)
		return false;

	CKernel* kernel=gui->guikernel.get_kernel();
	if (!kernel)
		SG_SERROR("No kernel defined.\n");

	switch (kernel->get_kernel_type())
	{
		case K_WEIGHTEDDEGREEPOS:
		{
			if (m_nrhs!=2)
				SG_SERROR("parameter missing\n");

			INT max_order=get_int();
			if ((max_order<1) || (max_order>12))
			{
				SG_WARNING( "max_order out of range 1..12 (%d). setting to 1\n", max_order);
				max_order=1;
			}

			CWeightedDegreePositionStringKernel* k=(CWeightedDegreePositionStringKernel*) kernel;
			CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
			if (!svm)
				SG_SERROR("No SVM defined.\n");

			INT num_suppvec=svm->get_num_support_vectors();
			INT* sv_idx=new INT[num_suppvec];
			DREAL* sv_weight=new DREAL[num_suppvec];
			INT num_feat=0;
			INT num_sym=0;

			for (INT i=0; i<num_suppvec; i++)
			{
				sv_idx[i]=svm->get_support_vector(i);
				sv_weight[i]=svm->get_alpha(i);
			}

			DREAL* position_weights=k->extract_w(max_order, num_feat,
				num_sym, NULL, num_suppvec, sv_idx, sv_weight);
			delete[] sv_idx;
			delete[] sv_weight;

			set_real_matrix(position_weights, num_sym, num_feat);
			delete[] position_weights;

			return true;
		}

		case K_COMMWORDSTRING:
		case K_WEIGHTEDCOMMWORDSTRING:
		{
			CCommWordStringKernel* k=(CCommWordStringKernel*) kernel;
			INT len=0;
			DREAL* weights;
			k->get_dictionary(len, weights);

			set_real_vector(weights, len);
			delete[] weights;

			return true;
		}
		case K_LINEAR:
		{
			CLinearKernel* k=(CLinearKernel*) kernel;
			INT len=0;
			const double* weights=k->get_normal(len);

			set_real_vector(weights, len);

			return true;
		}
		case K_SPARSELINEAR:
		{
			CSparseLinearKernel* k=(CSparseLinearKernel*) kernel;
			INT len=0;
			const double* weights=k->get_normal(len);

			set_real_vector(weights, len);

			return true;
		}
		default:
			SG_SERROR("Unsupported kernel %s.\n", kernel->get_name());
	}

	return true;
}

bool CSGInterface::a_plugin_estimate_classify_example()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT idx=get_int();
	DREAL result=gui->guipluginestimate.classify_example(idx);

	set_real_vector(&result, 1);
	return true;
}

bool CSGInterface::a_plugin_estimate_classify()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CFeatures* feat=gui->guifeatures.get_test_features();
	if (!feat)
		SG_SERROR("No features found.\n");

	INT num_vec=feat->get_num_vectors();
	DREAL* result=new DREAL[num_vec];
	ASSERT(result);

	CLabels* labels=gui->guipluginestimate.classify();
	for (INT i=0; i<num_vec; i++)
		result[i]=labels->get_label(i);
	delete labels;

	set_real_vector(result, num_vec);
	delete[] result;

	return true;
}

bool CSGInterface::a_set_plugin_estimate()
{
	if (m_nlhs!=0 || m_nrhs!=3)
		return false;

	DREAL* emission_probs=NULL;
	INT num_probs=0;
	INT num_vec=0;
	get_real_matrix(emission_probs, num_probs, num_vec);

	if (num_vec!=2)
	{
		delete[] emission_probs;
		SG_SERROR("Need at least 1 set of positive and 1 set of negative params.\n");
	}

	DREAL* pos_params=emission_probs;
	DREAL* neg_params=&(emission_probs[num_probs]);

	DREAL* model_sizes=NULL;
	INT len=0;
	get_real_vector(model_sizes, len);

	INT seq_length=(INT) model_sizes[0];
	INT num_symbols=(INT) model_sizes[1];
	if (num_probs!=seq_length*num_symbols)
	{
		delete[] emission_probs;
		delete[] model_sizes;
		SG_SERROR("Mismatch in number of emission probs and sequence length * number of symbols.\n");
	}

	gui->guipluginestimate.get_estimator()->set_model_params(
		pos_params, neg_params, seq_length, num_symbols);

	delete[] emission_probs;
	delete[] model_sizes;
	return true;
}

bool CSGInterface::a_get_plugin_estimate()
{
	if (m_nlhs!=2 || m_nrhs!=1)
		return false;

	DREAL* pos_params=NULL;
	DREAL* neg_params=NULL;
	INT num_params=0;
	INT seq_length=0;
	INT num_symbols=0;

	if (!gui->guipluginestimate.get_estimator()->get_model_params(
		pos_params, neg_params, seq_length, num_symbols))
		return false;

	num_params=seq_length*num_symbols;

	DREAL* result=new DREAL[num_params*2];
	ASSERT(result);

	for (INT i=0; i<num_params; i++)
		result[i]=pos_params[i];
	for (INT i=0; i<num_params; i++)
		result[i+num_params]=neg_params[i];

	set_real_matrix(result, num_params, 2);
	delete[] result;

	DREAL model_sizes[2];
	model_sizes[0]=(DREAL) seq_length;
	model_sizes[1]=(DREAL) num_symbols;
	set_real_vector(model_sizes, 2);

	return true;
}

bool CSGInterface::a_classify()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CFeatures* feat=gui->guifeatures.get_test_features();
	if (!feat)
		SG_SERROR("No features found.\n");

	INT num_vec=feat->get_num_vectors();
	CLabels* labels=gui->guiclassifier.classify();
	if (!labels)
		SG_SERROR("Classify failed\n");

	DREAL* result=new DREAL[num_vec];
	ASSERT(result);

	for (INT i=0; i<num_vec; i++)
		result[i]=labels->get_label(i);
	delete labels;

	set_real_vector(result, num_vec);
	delete[] result;

	return true;
}

bool CSGInterface::a_classify_example()
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT idx=get_int();
	DREAL result=0;

	if (!gui->guiclassifier.classify_example(idx, result))
		SG_SERROR("Classify_example failed.\n");

	set_real_vector(&result, 1);

	return true;
}

bool CSGInterface::a_get_classifier()
{
	if (m_nlhs!=2 || m_nrhs!=1)
		return false;

	DREAL* bias=NULL;
	DREAL* weights=NULL;
	INT rows=0;
	INT cols=0;
	INT brows=0;
	INT bcols=0;

	if (!gui->guiclassifier.get_trained_classifier(weights, rows, cols, bias, brows, bcols))
		return false;

	set_real_matrix(bias, brows, bcols);
	delete[] bias;
	set_real_matrix(weights, rows, cols);
	delete[] weights;

	return true;
}

bool CSGInterface::a_get_svm()
{
	return a_get_classifier();
}

bool CSGInterface::a_set_svm()
{
	if (m_nlhs!=0 || m_nrhs!=3)
		return false;

	DREAL bias=get_real();
	DREAL* alphas=NULL;
	INT num_feat_alphas=0;
	INT num_vec_alphas=0;
	get_real_matrix(alphas, num_feat_alphas, num_vec_alphas);

	if (!alphas)
		SG_SERROR("No proper alphas given.\n");
	if (num_vec_alphas!=2)
	{
		delete[] alphas;
		SG_SERROR("Not 2 vectors in alphas.\n");
	}

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	if (!svm)
		SG_SERROR("No SVM object available.\n");

	svm->create_new_model(num_feat_alphas);
	svm->set_bias(bias);

	INT num_support_vectors=svm->get_num_support_vectors();
	for (INT i=0; i<num_support_vectors; i++)
	{
		svm->set_alpha(i, alphas[i]);
		svm->set_support_vector(i, (INT) alphas[i+num_support_vectors]);
	}

	delete[] alphas;
	return true;
}

bool CSGInterface::a_get_svm_objective()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CSVM* svm=(CSVM*) gui->guiclassifier.get_classifier();
	if (!svm)
		SG_SERROR("No SVM set.\n");

	DREAL objective=svm->get_objective();
	set_real_vector(&objective, 1);

	return true;
}

bool CSGInterface::a_relative_entropy()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CHMM* pos=gui->guihmm.get_pos();
	CHMM* neg=gui->guihmm.get_neg();
	if (!pos || !neg)
		//return false;
		SG_SERROR("Set pos and neg HMM first!\n");

	INT pos_N=pos->get_N();
	INT neg_N=neg->get_N();
	INT pos_M=pos->get_M();
	INT neg_M=neg->get_M();
	if (pos_M!=neg_M || pos_N!=neg_N)
		//return false;
		SG_SERROR("Pos and neg HMM's differ in number of emissions or states.\n");

	DREAL* p=new DREAL[pos_M];
	ASSERT(p);
	DREAL* q=new DREAL[neg_M];
	ASSERT(q);
	DREAL* entropy=new DREAL[pos_N];
	ASSERT(entropy);

	for (INT i=0; i<pos_N; i++)
	{
		for (INT j=0; j<pos_M; j++)
		{
			p[j]=pos->get_b(i, j);
			q[j]=neg->get_b(i, j);
		}

		entropy[i]=CMath::relative_entropy(p, q, pos_M);
	}
	delete[] p;
	delete[] q;

	set_real_vector(entropy, pos_N);
	delete[] entropy;

	return true;
}

bool CSGInterface::a_entropy()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CHMM* current=gui->guihmm.get_current();
	if (!current)
		//return false;
		SG_SERROR("Create HMM first!\n");

	INT N=current->get_N();
	INT M=current->get_M();
	DREAL* p=new DREAL[M];
	ASSERT(p);
	DREAL* entropy=new DREAL[N];
	ASSERT(entropy);

	for (INT i=0; i<N; i++)
	{
		for (INT j=0; j<M; j++)
			p[j]=current->get_b(i, j);

		entropy[i]=CMath::entropy(p, M);
	}
	delete[] p;

	set_real_vector(entropy, N);
	delete[] entropy;

	return true;
}

bool CSGInterface::a_hmm_classify()
{
	return do_hmm_classify(false, false);
}

bool CSGInterface::a_one_class_hmm_classify()
{
	return do_hmm_classify(false, true);
}

bool CSGInterface::a_one_class_linear_hmm_classify()
{
	return do_hmm_classify(true, true);
}

bool CSGInterface::do_hmm_classify(bool linear, bool one_class)
{
	if (m_nlhs!=1 || m_nrhs>1)
		return false;

	CFeatures* feat=gui->guifeatures.get_test_features();
	if (!feat)
		return false;

	INT num_vec=feat->get_num_vectors();
	CLabels* labels=NULL;

	if (linear) // must be one_class as well
	{
		labels=gui->guihmm.linear_one_class_classify();
	}
	else
	{
		if (one_class)
			labels=gui->guihmm.one_class_classify();
		else
			labels=gui->guihmm.classify();
	}
	if (!labels)
		return false;

	DREAL* result=new DREAL[num_vec];
	ASSERT(result);

	for (INT i=0; i<num_vec; i++)
		result[i]=labels->get_label(i);
	delete labels;

	set_real_vector(result, num_vec);
	delete[] result;

	return true;
}

bool CSGInterface::a_one_class_hmm_classify_example()
{
	return do_hmm_classify_example(true);
}

bool CSGInterface::a_hmm_classify_example()
{
	return do_hmm_classify_example(false);
}

bool CSGInterface::do_hmm_classify_example(bool one_class)
{
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

	INT idx=get_int();
	DREAL result=0;

	if (one_class)
		result=gui->guihmm.one_class_classify_example(idx);
	else
		result=gui->guihmm.classify_example(idx);

	set_real_vector(&result, 1);

	return true;
}

bool CSGInterface::a_hmm_likelihood()
{
	if (m_nlhs!=1 || m_nrhs!=1)
		return false;

	CHMM* h=gui->guihmm.get_current();
	if (!h)
		SG_SERROR("No HMM.\n");

	DREAL likelihood=h->model_probability();
	set_real_vector(&likelihood, 1);

	return true;
}

bool CSGInterface::a_get_viterbi_path()
{
	if (m_nlhs!=2 || m_nrhs!=2)
		return false;

	INT dim=get_int();
	SG_DEBUG("dim: %f\n", dim);

	CHMM* h=gui->guihmm.get_current();
	if (!h)
		return false;

	CFeatures* feat=gui->guifeatures.get_test_features();
	if (!feat || (feat->get_feature_class()!=C_STRING) ||
			(feat->get_feature_type()!=F_WORD))
		return false;

	h->set_observations((CStringFeatures<WORD>*) feat);

	INT num_feat=0;
	WORD* vec=((CStringFeatures<WORD>*) feat)->get_feature_vector(dim, num_feat);
	if (!vec || num_feat<=0)
		return false;

	SG_DEBUG( "computing viterbi path for vector %d (length %d)\n", dim, num_feat);
	DREAL likelihood=0;
	DREAL* path=(DREAL*) h->get_path(dim, likelihood);

	set_real_vector(path, num_feat);
	delete[] path;
	set_real_vector(&likelihood, 1);

	return true;
}

bool CSGInterface::a_append_hmm()
{
	if (m_nlhs!=0 || m_nrhs!=5)
		return false;

	CHMM* old_h=gui->guihmm.get_current();
	if (!old_h)
		SG_SERROR("No current HMM set.\n");

	DREAL* p=NULL;
	INT M_p=0;
	INT N_p=0;
	get_real_matrix(p, M_p, N_p);

	DREAL* q=NULL;
	INT M_q=0;
	INT N_q=0;
	get_real_matrix(q, M_q, N_q);

	DREAL* a=NULL;
	INT M_a=0;
	INT N_a=0;
	get_real_matrix(a, M_a, N_a);
	INT N=N_a;

	DREAL* b=NULL;
	INT M_b=0;
	INT N_b=0;
	get_real_matrix(b, M_b, N_b);
	INT M=N_b;

	SG_DEBUG("p:(%d,%d) q:(%d,%d) a:(%d,%d) b(%d,%d)\n",
		N_p, M_p, N_q, M_q, N_a, M_a, N_b, M_b);
	if (N_p!=N || M_p!=1 || N_q!=N || M_q!=1 ||
		N_a!=N || M_a!=N || N_b!=M || M_b!=N)
	{
		delete[] p;
		delete[] q;
		delete[] a;
		delete[] b;
		SG_SERROR("Model matrices not matching in size.\n");
	}

	CHMM* h=new CHMM(N, M, NULL, gui->guihmm.get_pseudo());
	ASSERT(h);
	INT i,j;

	for (i=0; i<N; i++)
	{
		h->set_p(i, p[i]);
		h->set_q(i, q[i]);
	}

	for (i=0; i<N; i++)
		for (j=0; j<N; j++)
			h->set_a(i,j, a[i+j*N]);

	for (i=0; i<N; i++)
		for (j=0; j<M; j++)
			h->set_b(i,j, b[i+j*N]);

	old_h->append_model(h);
	delete h;

	return true;
}

bool CSGInterface::a_set_hmm()
{
	if (m_nlhs!=0 || m_nrhs!=5)
		return false;

	DREAL* p=NULL;
	INT M_p=0;
	INT N_p=0;
	get_real_matrix(p, M_p, N_p);

	DREAL* q=NULL;
	INT M_q=0;
	INT N_q=0;
	get_real_matrix(q, M_q, N_q);

	DREAL* a=NULL;
	INT M_a=0;
	INT N_a=0;
	get_real_matrix(a, M_a, N_a);
	INT N=N_a;

	DREAL* b=NULL;
	INT M_b=0;
	INT N_b=0;
	get_real_matrix(b, M_b, N_b);
	INT M=N_b;

	SG_DEBUG("p:(%d,%d) q:(%d,%d) a:(%d,%d) b(%d,%d)\n",
		N_p, M_p, N_q, M_q, N_a, M_a, N_b, M_b);
	if (N_p!=N || M_p!=1 || N_q!=N || M_q!=1 ||
		N_a!=N || M_a!=N || N_b!=M || M_b!=N)
	{
		delete[] p;
		delete[] q;
		delete[] a;
		delete[] b;
		SG_SERROR("Model matrices not matching in size.\n");
	}

	CHMM* h=new CHMM(N, M, NULL, gui->guihmm.get_pseudo());
	ASSERT(h);
	INT i,j;

	for (i=0; i<N; i++)
	{
		h->set_p(i, p[i]);
		h->set_q(i, q[i]);
	}

	for (i=0; i<N; i++)
		for (j=0; j<N; j++)
			h->set_a(i,j, a[i+j*N]);

	for (i=0; i<N; i++)
		for (j=0; j<M; j++)
			h->set_b(i,j, b[i+j*N]);

	CHMM* current=gui->guihmm.get_current();
	if (current)
		delete current;

	gui->guihmm.set_current(h);

	return true;
}

bool CSGInterface::a_get_hmm()
{
	if (m_nlhs!=4 || m_nrhs!=1)
		return false;

	CHMM* h=gui->guihmm.get_current();
	if (!h)
		return false;

	INT N=h->get_N();
	INT M=h->get_M();
	INT i=0;
	INT j=0;
	DREAL* p=new DREAL[N];
	ASSERT(p);
	DREAL* q=new DREAL[N];
	ASSERT(q);

	for (i=0; i<N; i++)
	{
		p[i]=h->get_p(i);
		q[i]=h->get_q(i);
	}

	set_real_vector(p, N);
	delete[] p;
	set_real_vector(q, N);
	delete[] q;

	DREAL* a=new DREAL[N*N];
	ASSERT(a);
	for (i=0; i<N; i++)
		for (j=0; j<N; j++)
			a[i+j*N]=h->get_a(i, j);
	set_real_matrix(a, N, N);
	delete[] a;

	DREAL* b=new DREAL[N*M];
	ASSERT(b);
	for (i=0; i<N; i++)
		for (j=0; j<M; j++)
			b[i+j*N]=h->get_b(i, j);

	set_real_matrix(b, N, M);
	delete[] b;

	return true;
}

bool CSGInterface::a_help()
{
	if (m_nrhs!=1 || m_nlhs!=0)
		return false;

	gui->print_help();

	return true;
}

bool CSGInterface::a_test()
{
	SG_PRINT("entering test method\n");
	if (m_nlhs!=1 || m_nrhs!=2)
		return false;

/*
	CSGInterfaceVector iv(SGIDT_CHAR);
	get_vector(iv);
//	const DREAL *vector=NULL;
//	INT len=0;
//	iv.get(vector, len);
//	for (INT i=0; i<len; i++) SG_PRINT("data %d: %f\n", i, vector[i]);
	set_vector(iv);
*/

/*
	CSGInterfaceMatrix im(SGIDT_CHAR);
	get_matrix(im);
//	const DREAL* matrix=NULL;
//	UINT m=0;
//	UINT n=0;;
//	im.get(matrix, m, n);
//	for (UINT i=0; i<m; i++)
//		for (UINT j=0; j<n; j++)
//			SG_PRINT("data %d, %d, %f\n", i, j, matrix[i*n+j]);
	set_matrix(im);
*/


/*
	CSGInterfaceMatrix im(SGIDT_SPARSEDREAL);
	get_sparsematrix(im);
	const TSparse<DREAL>* matrix=NULL;
	UINT num_feat=0;
	UINT num_vec=0;
	im.get(matrix, num_feat, num_vec);
	//SG_PRINT("M %d, N %d\n", num_feat, num_vec);
	//for (UINT i=0; i<num_vec; i++)
	//	for (UINT j=0; j<num_feat; j++)
	//		SG_PRINT("data %d, %d, %d\n", i, j, matrix[i].features[j].entry);
	set_sparsematrix(im);

	for (UINT i=0; i<num_vec; i++)
	{
		if (matrix[i].features)
			delete[] matrix[i].features;
	}
	delete[] matrix;
*/


	CSGInterfaceStringList isl(SGIDT_CHAR);
	get_string_list(isl);
//	SG_PRINT("Got it\n");
//	const T_STRING<CHAR>* list=NULL;
//	UINT num_str=0;
//	isl.get(list, num_str);
//	SG_PRINT("num %d\n", num_str);
//	for (UINT i=0; i<num_str; i++)
//		SG_PRINT("string: %s", list[i].string);
	set_string_list(isl);
//	delete[] list;

	return true;
}


////////////////////////////////////////////////////////////////////////////
// simple get helper
////////////////////////////////////////////////////////////////////////////

INT CSGInterface::get_int_from_string()
{
	INT len=0;
	CHAR* str=get_string(len);
	return strtol(str, NULL, 10);
}

DREAL CSGInterface::get_real_from_string()
{
	INT len=0;
	CHAR* str=get_string(len);
	return strtod(str, NULL);
}

bool CSGInterface::get_bool_from_string()
{
	INT len=0;
	CHAR* str=get_string(len);
	return strtol(str, NULL, 10)!=0;
}


////////////////////////////////////////////////////////////////////////////
// handler
////////////////////////////////////////////////////////////////////////////

bool CSGInterface::handle()
{
	INT len=0;
	bool success=false;

#ifndef WIN32
	CSignal::set_handler();
#endif

	if (!gui)
		gui=new CTextGUI(0, NULL);
	if (!gui)
		SG_SERROR("GUI could not be initialized.\n");

	CHAR* action=NULL;
	try
	{
		action=interface->get_action(len);
	}
	catch (ShogunException e)
	{
		SG_SERROR("String expected as first argument: %s\n", e.get_exception_string());
	}

	SG_PRINT("action: %s, nlhs %d, nrhs %d\n", action, m_nlhs, m_nrhs);
	INT i=0;
	while (sg_methods[i].action)
	{
		if (strmatch(action, len, sg_methods[i].action))
		{
			SG_DEBUG("found method %s\n", sg_methods[i].action);
			if (!(interface->*(sg_methods[i].method))())
				SG_SERROR("Usage: %s\n", sg_methods[i].usage);
			else
			{
				success=true;
				break;
			}
		}
		i++;
	}

	// FIXME: invoke old interface
	if(!success && strmatch(action, len, N_SEND_COMMAND))
	{
		//parse_args(2, 0);
		CHAR* cmd=interface->get_string(len);
		SG_PRINT("cmd:%s\n", cmd);
		gui->parse_line(cmd);

		delete[] cmd;
		delete gui;
		success=true;
	}

#ifndef WIN32
	CSignal::unset_handler();
#endif

	delete[] action;
	return success;
}

#endif // !HAVE_SWIG