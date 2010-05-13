/****************************************************************
 *                                                              *
 * context.h - the definition of context                        *
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2007.12                        *
 *                                                              *
 ****************************************************************/

class CStateNode; 
class CStateItem;

const CConstituent g_noneConstituent(CConstituent::NONE);
const CConstituent g_beginConstituent(CConstituent::SENTENCE_BEGIN);
const CTag g_noneTag(CTag::NONE);

#define constituent_or_none(x) (x.is_constituent ? x.constituent : CConstituent::NONE)

//===============================================================
//
// CContext 
//
//===============================================================

class CContext {
public:
   int s0, s1, s2, s3;
   int n0, n1, n2, n3;
   int s0l, s0r, s0u, s0h;
   int s1l, s1r, s1u, s1h;
   int s0ld, s0rd;
   int s1ld, s1rd;

   const CWord *s0w, *s1w, *s2w, *s3w;
   CConstituent s0c, s1c, s2c, s3c;
   CTag s0t, s1t, s2t, s3t;

   const CWord *n0w, *n1w, *n2w, *n3w;
   CTag n0t, n1t, n2t, n3t;

   const CWord *s0lw, *s0rw, *s0uw, *s1lw, *s1rw, *s1uw;
   CConstituent s0lc, s0rc, s0uc;
   CTag s0lt, s0rt, s0ut;
   CConstituent s1lc, s1rc, s1uc;
   CTag s1lt, s1rt, s1ut;

   unsigned long s0ldt, s0rdt;
   unsigned long s1ldt, s1rdt;
   const CTaggedWord<CTag, TAG_SEPARATOR> *s0wt, *s1wt, *n0wt, *n1wt, *n2wt, *n3wt;
   const CWord *s0ldw, *s0rdw, *s1ldw, *s1rdw;

   unsigned long stacksize;


   CCFGSet s0cs1c, s0ts1t, s0cn0t, s1cn0t, n0tn1t, s0tn0t, 
           s0cs1cn0t, s0cn0tn1t, s0cs1cs2c, 
           s0ts1tn0t, s0tn0tn1t, s0ts1ts2t,
           s0js1jn0t, s0jn0tn1t, s0js1js2j;
   // here j means the combination of c and t -- t with non-constituent
   CCFGSet s0cs0lc, s0cs0rc, s1cs1rc, 
           s0cs0rcn0t, s0cs0rjn0t, s0cs0lcs1c, s0cs0ljs1j, s0cs1cs1rc, s0js1cs1rj;
   CTwoWords s0ws1w, s0wn0w, n0wn1w, s1wn0w;
   CTwoTaggedWords s0wts1wt;

   vector<CTag> between_tag;
   CCFGSet s0ts1tbt;

   // sub nodes unbinarized for the binarized 
   vector<unsigned long> s0_unbinarized;
   vector<unsigned long> s1_unbinarized;
   vector<CConstituent> s0_unbinarized_cs;
   vector<CConstituent> s1_unbinarized_cs;
   // the head child
   CConstituent s0hc, s1hc;
   // the constituent, its head child, and the left/right two ones
   // applied to any constituent no matter how many sub-nodes
   CCFGSet s0cs0hcs0r2c, s0cs0hcs0l2c, s1cs1hcs1r2c;
   // unpacked left/right nodes for those having >=2
   CCFGSet s0r6c, s0l6c, s1r6c;
   int s0h_unbinarized; // the head node among all unexpanded nodes from s0
   int s1h_unbinarized; // the head node among all unexpanded nodes from s1
   int s0h_unbinarized_index;
   int s1h_unbinarized_index;

   unsigned long open_bracket_match_type;
#ifdef _CHINESE_CFG_H
   unsigned long s0c_bracket, s1c_bracket, n0t_bracket, s0cs1c_bracket, s0cn0t_bracket;

   vector<unsigned long> s0cs1c_separator; // one particular separator punctuation
   vector<unsigned long> s0c_separator;
   vector<unsigned long> s1c_separator;
   unsigned long s0cs1c_sepcount; // the count of separator pus between them
   unsigned long s0c_sepcount; // unigram backup
   unsigned long s1c_sepcount;
#endif

   unsigned long s0ln, s0rn, s1ln, s1rn;
   unsigned long s0m, s1m, s2m, n0m;
   bool n0norv;

   unsigned long s0s1_dist, s0cs1_dist, s0s1c_dist, s0cs1c_dist;
protected:
   // protected because they're transient
   const CStateNode *s0node, *s1node, *s2node, *s3node;
//public:
//   CContext() {}
//   ~CContext() {}
public:
   unsigned long unbinarize(const vector<CStateNode> &nodes, const unsigned long &curr, vector<unsigned long> &rval) {
      unsigned long head;
      head = curr;
      if (nodes[curr].temp) {
         assert(nodes[curr].is_constituent&&!nodes[curr].single_child);
         if (nodes[curr].head_left) {
            head = unbinarize(nodes, nodes[curr].left_child, rval);
            assert(!nodes[nodes[curr].right_child].temp);
            rval.push_back(nodes[curr].right_child);
         }
         else {
            rval.push_back(nodes[curr].left_child);
            assert(!nodes[nodes[curr].left_child].temp);
            head = unbinarize(nodes, nodes[curr].right_child, rval);
         }
      }
      else {
         rval.push_back(curr);
      }
      return head;
   }
   void countleftdependents(const vector<CStateNode> &nodes, const unsigned long &i, int &last, unsigned long &count) {
      if (!nodes[i].is_constituent) {
         last = -1;
         count = 0;
      }
      else {
         if (nodes[i].single_child) {
            countleftdependents(nodes, nodes[i].left_child, last, count);
         }
         else {
            if (nodes[i].head_left) {
               countleftdependents(nodes, nodes[i].left_child, last, count);
            }
            else {
               countleftdependents(nodes, nodes[i].right_child, last, count) ;
               count ++;
               last = nodes[i].left_child;
            }
         }
      }
   }
   void countrightdependents(const vector<CStateNode> &nodes, const unsigned long &i, int &last, unsigned long &count) {
      if (!nodes[i].is_constituent) {
         last = -1;
         count = 0;
      }
      else {
         if (nodes[i].single_child) {
            countrightdependents(nodes, nodes[i].left_child, last, count);
         }
         else {
            if (nodes[i].head_left) {
               countrightdependents(nodes, nodes[i].left_child, last, count);
               count ++;
               last = nodes[i].right_child;
            }
            else {
               countrightdependents(nodes, nodes[i].right_child, last, count) ;
            }
         }
      }
   }
   void load(const CStateItem *item, const vector<CTaggedWord<CTag, TAG_SEPARATOR> > &wrds, const vector<unsigned long> &wordlen, const bool &modify) {
      static CStateNode emptyItem;
      stacksize = item->stack.size();
      if (stacksize==0) return; // must shift; no feature updates, no comparisons for different actions
      static unsigned long tmp;
      static int tmp_i;
      static int i, j;
      s0 = item->stack.back();
      s1 = stacksize<2 ? -1 : item->stack[stacksize-2];
      s2 = stacksize<3 ? -1 : item->stack[stacksize-3];
      s3 = stacksize<4 ? -1 : item->stack[stacksize-4];
      n0 = item->current_word >= wrds.size() ? -1 : item->current_word;
      n1 = item->current_word+1 >= wrds.size() ? -1 : item->current_word+1;
      n2 = item->current_word+2 >= wrds.size() ? -1 : item->current_word+2;
      n3 = item->current_word+3 >= wrds.size() ? -1 : item->current_word+3;
   
      s0node = &(item->nodes[s0]);
      s1node = s1==-1 ? 0 : &(item->nodes[s1]);
      s2node = s2==-1 ? 0 : &(item->nodes[s2]);
      s3node = s3==-1 ? 0 : &(item->nodes[s3]);
  
      s0l = s0node->is_constituent ? (s0node->single_child||s0node->head_left ? -1 : s0node->left_child) : -1;
      s0r = s0node->is_constituent ? (s0node->single_child||!s0node->head_left ? -1 : s0node->right_child) : -1;
      s0u = s0node->is_constituent ? (s0node->single_child ? s0node->left_child : -1) : -1;
      s0h = s0node->is_constituent ? (s0node->single_child||s0node->head_left ? s0node->left_child : s0node->right_child) : -1;

      s1l = s1==-1 ? -1 : ( s1node->is_constituent ?  (s1node->single_child||s1node->head_left ? -1 : s1node->left_child) : -1 );
      s1r = s1==-1 ? -1 : ( s1node->is_constituent ? (s1node->single_child||!s1node->head_left ? -1 : s1node->right_child) : -1 );
      s1u = s1==-1 ? -1 : ( s1node->is_constituent ? (s1node->single_child ? s1node->left_child : -1) : -1 );
      s1l = s1==-1 ? -1 : ( s1node->is_constituent ?  (s1node->single_child||s1node->head_left ? s1node->left_child : s1node->right_child) : -1 );
   
      s0c.load(s0node->is_constituent ? s0node->constituent : CConstituent::NONE);
      s0wt = &(wrds[s0node->lexical_head]);
      s0w = s0wt;
      s0t = s0wt->tag;
   
      s1c.load(s1==-1 ? CConstituent::SENTENCE_BEGIN : s1node->is_constituent ? s1node->constituent : CConstituent::NONE);
      s1wt = s1 == -1 ? 0 : &(wrds[s1node->lexical_head]);
      s1w = s1wt;
      s1t = s1 == -1 ? g_noneTag : s1wt->tag;
   
      s2c.load(s2==-1 ? CConstituent::SENTENCE_BEGIN : s2node->is_constituent ? s2node->constituent : CConstituent::NONE);
      s2w = s2 == -1 ? 0 : &(wrds[s2node->lexical_head]);
      s2t = s2 == -1 ? 0 : wrds[s2node->lexical_head].tag;
   
      s3c.load(s3==-1 ? CConstituent::SENTENCE_BEGIN : s3node->is_constituent ? s3node->constituent : CConstituent::NONE);
      s3w = s3 == -1 ? 0 : &(wrds[s3node->lexical_head]);
      s3t = s3 == -1 ? 0 : wrds[s3node->lexical_head].tag;
   
      s0lc.load(s0l==-1 ? CConstituent::SENTENCE_BEGIN  
                        : item->nodes[s0l].is_constituent ? item->nodes[s0l].constituent 
                                                          : CConstituent::NONE);
      s0lw = s0l==-1 ? 0 : &(wrds[item->nodes[s0l].lexical_head]);
      s0lt = s0l==-1 ? g_noneTag : wrds[item->nodes[s0l].lexical_head].tag;
      s0rc.load(s0r==-1 ? CConstituent::SENTENCE_BEGIN : item->nodes[s0r].is_constituent ? item->nodes[s0r].constituent : CConstituent::NONE);
      s0rw = s0r==-1 ? 0 : &(wrds[item->nodes[s0r].lexical_head]);
      s0rt = s0r==-1 ? 0 : wrds[item->nodes[s0r].lexical_head].tag;
      s0uc.load(s0u==-1 ? CConstituent::SENTENCE_BEGIN : item->nodes[s0u].is_constituent ? item->nodes[s0u].constituent : CConstituent::NONE);
      s0uw = s0u==-1 ? 0 : &(wrds[item->nodes[s0u].lexical_head]);
      s0ut = s0u==-1 ? 0 : wrds[item->nodes[s0u].lexical_head].tag;
   
      s1lc.load(s1l==-1 ? CConstituent::SENTENCE_BEGIN : item->nodes[s1l].is_constituent ? item->nodes[s1l].constituent : CConstituent::NONE);
      s1lw = s1l==-1 ? 0 : &(wrds[item->nodes[s1l].lexical_head]);
      s1lt = s1l==-1 ? 0 : wrds[item->nodes[s1l].lexical_head].tag;
      s1rc.load(s1r==-1 ? CConstituent::SENTENCE_BEGIN : item->nodes[s1r].is_constituent ? item->nodes[s1r].constituent : CConstituent::NONE);
      s1rw = s1r==-1 ? 0 : &(wrds[item->nodes[s1r].lexical_head]);
      s1rt = s1r==-1 ? 0 : wrds[item->nodes[s1r].lexical_head].tag;
      s1uc.load(s1u==-1 ? CConstituent::SENTENCE_BEGIN : item->nodes[s1u].is_constituent ? item->nodes[s1u].constituent : CConstituent::NONE);
      s1uw = s1u==-1 ? 0 : &(wrds[item->nodes[s1u].lexical_head]);
      s1ut = s1u==-1 ? 0 : wrds[item->nodes[s1u].lexical_head].tag;

      n0t = n0==-1 ? g_noneTag : wrds[n0].tag;
      n0w = n0 == -1 ? 0 : &(wrds[n0]);
      n0wt = n0 == -1 ? 0 : &(wrds[n0]);
   
      n1t = n1==-1 ? g_noneTag : wrds[n1].tag;
      n1w = n1 == -1 ? 0 : &(wrds[n1]);
      n1wt = n1 == -1 ? 0 : &(wrds[n1]);
   
      n2t = n2==-1 ? g_noneTag : wrds[n2].tag;
      n2w = n2 == -1 ? 0 : &(wrds[n2]);
      n2wt = n2 == -1 ? 0 : &(wrds[n2]);
 
      n3t = n3==-1 ? g_noneTag : wrds[n3].tag;
      n3w = n3 == -1 ? 0 : &(wrds[n3]);
      n3wt = n3 == -1 ? 0 : &(wrds[n3]);

      if (modify==false) {
         if (s1!=-1) s0ws1w.refer(s0w, s1w); 
         if (n0!=-1) s0wn0w.refer(s0w, n0w); 
         if (n1!=-1) n0wn1w.refer(n0w, n1w);
         if (s1!=-1&&n0!=-1) s1wn0w.refer(s1w, n0w);
         if (s1!=-1) s0wts1wt.refer(s0wt, s1wt); 
      }
      else {
         if (s1!=-1) s0ws1w.allocate(*s0w, *s1w);
         if (n0!=-1) s0wn0w.allocate(*s0w, *n0w);
         if (n1!=-1) n0wn1w.allocate(*n0w, *n1w);
         if (s1!=-1&&n0!=-1) s1wn0w.allocate(*s1w, *n0w);
         if (s1!=-1) s0wts1wt.allocate(*s0wt, *s1wt);
      }
      s0cs1c.load(s0c, s1c);
      s0ts1t.load(s0t, s1t);
      s0cn0t.load(s0c, n0t);
      n0tn1t.load(n0t, n1t);
      s1cn0t.load(s1c, n0t);
      s0tn0t.load(s0t, n0t);
      s0cs1cn0t.copy(s0cs1c); s0cs1cn0t.add(n0t);
      s0cs1cs2c.copy(s0cs1c); s0cs1cs2c.add(s2c);
      s0cn0tn1t.copy(s0cn0t); s0cn0tn1t.add(n1t);
      s0ts1tn0t.copy(s0ts1t); s0ts1tn0t.add(n0t);
      s0ts1ts2t.copy(s0ts1t); s0ts1ts2t.add(s2t);
      s0tn0tn1t.copy(s0tn0t); s0tn0tn1t.add(n1t);

      if (s0c.empty()) s0js1jn0t.load(s0t); else s0js1jn0t.load(s0c);
      if (s1c.empty()) s0js1jn0t.add(s1t); else s0js1jn0t.add(s1c);
      s0js1jn0t.add(n0t);

      if (s0c.empty()) s0jn0tn1t.load(s0t); else s0jn0tn1t.load(s0c);
      s0jn0tn1t.add(n0t);
      s0jn0tn1t.add(n1t);

      if (s0c.empty()) s0js1js2j.load(s0t); else s0js1js2j.load(s0c);
      if (s1c.empty()) s0js1js2j.add(s1t); else s0js1js2j.add(s1c);
      if (s2c.empty()) s0js1js2j.add(s2t); else s0js1js2j.add(s2c);

      // s0 s0r and n0 -- by presuming that n0 exists!
      // s0rc is used when s0r exists, while s0rj is used all times
      // s0rc can be empty is s0r is not a constituent
      // s0rj is s0rc when s0rc not empty, and s0rt otherwise
      // s0rj empty only when s0r non existant
      s0cs0rc.load(s0c, s0rc);
      s0cs0rcn0t.copy(s0cn0t); s0cs0rcn0t.add(s0rc);

      s0cs0rjn0t.copy(s0cn0t); 
      if (s0rc.empty()) s0cs0rjn0t.add(s0rt); else s0cs0rjn0t.add(s0rc);

      // s0 s0l and s1 -- by presuming that s1 exists!
      // see comments above
      s0cs0lc.load(s0c, s0lc);
      s0cs0lcs1c.copy(s0cs1c);
      s0cs0lcs1c.add(s0lc);

      s0cs0ljs1j.load(s0c);
      if (s0lc.empty()) s0cs0ljs1j.add(s0lt); else s0cs0ljs1j.add(s0lc);
      if (s1c.empty()) s0cs0ljs1j.add(s1t); else s0cs0ljs1j.add(s1c);

      // s0 slr and s1 -- by presuming that s1 exists!
      s1cs1rc.load(s1c, s1rc);
      s0cs1cs1rc.copy(s0cs1c);
      s0cs1cs1rc.add(s1rc);

      if (s0c.empty()) s0js1cs1rj.load(s0t); else s0js1cs1rj.load(s0c);
      s0js1cs1rj.add(s1c);
      if (s1rc.empty()) s0js1cs1rj.add(s1rt); else s0js1cs1rj.add(s1rc);

      // a tag between s0 and s1
      s0ts1tbt = s0ts1t; s0ts1tbt.add(g_noneTag);
      between_tag.clear();
      if (s1!=-1)
      for (i=s1node->lexical_head+1; i<s0node->lexical_head; ++i) {
         between_tag.push_back(wrds[i].tag);
      }

      // unexpand s0 sub
      s0_unbinarized.clear();
      s0_unbinarized_cs.clear();
      s0h_unbinarized = -1;
      if (s0l!=-1) s0_unbinarized.push_back(s0l); // leftmost sub
      if (s0h!=-1) { // head sub
         s0h_unbinarized = unbinarize(item->nodes, s0h, s0_unbinarized);
      }
      if (s0r!=-1) s0_unbinarized.push_back(s0r); // rightmost sub
      // collect tag
      s0h_unbinarized_index = -1;
      for (tmp=0; tmp<s0_unbinarized.size(); tmp++) {
         s0_unbinarized_cs.push_back(constituent_or_none(item->nodes[s0_unbinarized[tmp]]));
         if (s0_unbinarized[tmp]==s0h_unbinarized) s0h_unbinarized_index=tmp;
      }
      assert(s0_unbinarized.size()==0 || s0h_unbinarized_index!=-1);
      // unexpand s1 sub
      s1_unbinarized.clear();
      s1_unbinarized_cs.clear();
      s1h_unbinarized = -1;
      if (s1!=-1) {
         if (s1l!=-1) s1_unbinarized.push_back(s1l);
         if (s1h!=-1) {
            s1h_unbinarized = unbinarize(item->nodes, s1h, s1_unbinarized);
         }
         if (s1r!=-1) s1_unbinarized.push_back(s1r);
      }
      // collect tag for the unexpanded sub node constituent
      s1h_unbinarized_index=-1;
      if (s1!=-1)  {
         for(tmp=0; tmp<s1_unbinarized.size(); tmp++) {
            s1_unbinarized_cs.push_back(constituent_or_none(item->nodes[s1_unbinarized[tmp]]));
            if (s1_unbinarized[tmp]==s1h_unbinarized) s1h_unbinarized_index=tmp;
         }
      }
      assert(s1_unbinarized.size()==0 || s1h_unbinarized_index!=-1);

      // s0hc and s1hc
      s0hc.load(s0h_unbinarized_index!=-1 ? constituent_or_none(item->nodes[s0_unbinarized[s0h_unbinarized_index]]) : CConstituent::SENTENCE_BEGIN);
      s1hc.load(s1h_unbinarized_index!=-1 ? constituent_or_none(item->nodes[s1_unbinarized[s1h_unbinarized_index]]) : CConstituent::SENTENCE_BEGIN);

      // s0cs0hcs0l2c, s0cs0hcs0r2c, s1cs1hcs1r2c
      s0cs0hcs0r2c.load(s0c, s0hc);
      s0cs0hcs0r2c.add(s0h_unbinarized_index+1<s0_unbinarized.size() ? s0_unbinarized_cs.back() : g_beginConstituent);
      s0cs0hcs0r2c.add(s0h_unbinarized_index+2<s0_unbinarized.size() ? s0_unbinarized_cs[s0_unbinarized.size()-2] : g_beginConstituent);

      s0cs0hcs0l2c.load(s0c, s0hc);
      s0cs0hcs0l2c.add(s0h_unbinarized_index>0 ? s0_unbinarized_cs[0] : g_beginConstituent);
      s0cs0hcs0l2c.add(s0h_unbinarized_index>1 ? s0_unbinarized_cs[1] : g_beginConstituent);

      s1cs1hcs1r2c.load(s1c, s1hc);
      s1cs1hcs1r2c.add(s1h_unbinarized_index+1<s1_unbinarized.size() ? s1_unbinarized_cs.back() : g_beginConstituent);
      s1cs1hcs1r2c.add(s1h_unbinarized_index+2<s1_unbinarized.size() ? s1_unbinarized_cs[s1_unbinarized.size()-2] : g_beginConstituent);

      s0l6c.clear();
      if (s0_unbinarized.size()!=0) {
         for (tmp_i=0; tmp_i<6; ++tmp_i) {
            s0l6c.add(static_cast<int>(tmp_i)<s0h_unbinarized_index?
                      s0_unbinarized_cs[tmp_i]:g_beginConstituent);
         }
      }
      s0r6c.clear();
      if (s0_unbinarized.size()>0) {
         for (tmp_i=static_cast<int>(s0_unbinarized.size())-1; tmp_i>static_cast<int>(s0_unbinarized.size())-7; --tmp_i) {
            s0r6c.add(tmp_i>s0h_unbinarized_index?
                      s0_unbinarized_cs[tmp_i]:g_beginConstituent);
         }
      }
      s1r6c.clear();
      if (s1_unbinarized.size()>0) {
         for (tmp_i=static_cast<int>(s1_unbinarized.size())-1; tmp_i>static_cast<int>(s1_unbinarized.size())-7; --tmp_i) {
            s1r6c.add(tmp_i>s1h_unbinarized_index?s1_unbinarized_cs[tmp_i]:g_beginConstituent);
         }
      }

#ifdef _CHINESE_CFG_H
/*
      // bracket
      static int last_stack_starting_bracket;
      last_stack_starting_bracket = -1;
      open_bracket_match_type = 0; // null
      for (i=stacksize-1; i>=0; i--) {
         const CStateNode &node = item->nodes[item->stack[i]];
         last_stack_starting_bracket = node.is_constituent?-1:getStartingBracket(wrds[node.lexical_head]);
         if (last_stack_starting_bracket!=-1) { // find the first bracket
            open_bracket_match_type = 1; // open
            const int &rt = item->nodes[s0_unbinarized.back()].lexical_head;
            if (getEndingBracket(wrds[rt])==last_stack_starting_bracket) // match
               open_bracket_match_type = 2; // matched bracket
            else {
               for (j=node.lexical_head; j<rt; j++)
                  if (getEndingBracket(wrds[j])==last_stack_starting_bracket)
                     open_bracket_match_type = 3; // missed
            }
            break;
         }
      }
      s0c_bracket = encodeTorCs(open_bracket_match_type, s0c.code());   
      s1c_bracket = encodeTorCs(open_bracket_match_type, s1c.code());
      n0t_bracket = encodeTorCs(open_bracket_match_type, encodeTorC(n0t.code(), true));
      s0cs1c_bracket = encodeTorCs(open_bracket_match_type, s0c.code(), s1c.code());
      s0cn0t_bracket = encodeTorCs(open_bracket_match_type, s0c.code(), encodeTorC(n0t.code(), true));

      // comma, full stop, dun hao, semi colon;
      static int last_separator;
      static unsigned long sepcount;
      static unsigned long s0_sepset, s1_sepset, s0s1_sepset;
      s0cs1c_separator.clear();
      s0c_separator.clear();
      s1c_separator.clear();
      s0_sepset=0; s1_sepset=0; s0s1_sepset=0;
      sepcount = 0;
      if (s1!=-1) {
         for (i=s1node->lexical_head+1; i<s0node->lexical_head; ++i) {
            last_separator = getSeparatingPunctuation(wrds[i]);
            if (last_separator!=-1) {
               if ((s0s1_sepset&(1<<last_separator))==0) {
                  s0s1_sepset|=(1<<last_separator);
                  s0cs1c_separator.push_back(encodeTorCs(s0c.code(), s1c.code(), last_separator));
               }
               if ((s0_sepset&(1<<last_separator))==0) {
                  s0_sepset |= (1<<last_separator);
                  s0c_separator.push_back(encodeTorCs(s0c.code(), last_separator));
               }
               if ((s1_sepset & (1<<last_separator)) == 0) {
                  s1_sepset |= (1<<last_separator);
                  s1c_separator.push_back(encodeTorCs(s1c.code(), last_separator));
               }
               sepcount++ ;
            }
         }
      }
      s0cs1c_sepcount=encodeTorCs(s0c.code(), s1c.code(),sepcount);
      s0c_sepcount=encodeTorCs(s0c.code(),sepcount);
      s1c_sepcount=encodeTorCs(s1c.code(),sepcount);*/
#endif

      // S{0/1}{LD/RD}
      countleftdependents(item->nodes, s0, s0ld, s0ln);
      countrightdependents(item->nodes, s0, s0rd, s0rn);
      if (s1==-1) {
         s1ld = -1;
         s1rd = -1;
         s1ln = 0;
         s1rn = 0;
      }
      else {
         countleftdependents(item->nodes, s1, s1ld, s1ln);
         countrightdependents(item->nodes, s1, s1rd, s1rn);
      }
      s0ldt = s0ld==-1 ? 0 : wrds[item->nodes[s0ld].lexical_head].tag.code();
      s0ldw = s0ld == -1 ? 0 : &(wrds[item->nodes[s0ld].lexical_head]);
      s0rdt = s0rd==-1 ? 0 : wrds[item->nodes[s0rd].lexical_head].tag.code();
      s0rdw = s0rd == -1 ? 0 : &(wrds[item->nodes[s0rd].lexical_head]);
      s1ldt = s1ld==-1 ? 0 : wrds[item->nodes[s1ld].lexical_head].tag.code();
      s1ldw = s1ld == -1 ? 0 : &(wrds[item->nodes[s1ld].lexical_head]);
      s1rdt = s1rd==-1 ? 0 : wrds[item->nodes[s1rd].lexical_head].tag.code();
      s1rdw = s1rd == -1 ? 0 : &(wrds[item->nodes[s1rd].lexical_head]);

      if (s1!=-1) {
//         s0s1_dist = encodeLinkSize(s0node->lexical_head, s1node->lexical_head);
//         s0cs1_dist = encodeTorCs(s0c, s0s1_dist);
//         s0s1c_dist = encodeTorCs(s1c, s0s1_dist);
//         s0cs1c_dist = encodeTorCs(s0c.code(), s1c.code(), s0s1_dist);
      }

      return;
   }
};
                           
