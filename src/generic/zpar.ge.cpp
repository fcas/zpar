/****************************************************************
 *                                                              *
 * zpar.cpp - main app; a convenience wrapper for all algorithms*
 *                                                              *
 * Author: Yue Zhang                                            *
 *                                                              *
 * Computing Laboratory, Oxford. 2010.03                        *
 *                                                              *
 ****************************************************************/

#include "definitions.h"
#include "tagger.h"
#include "conparser.h"
#include "depparser.h"
#include "deplabeler.h"
#include "reader.h"
#include "writer.h"
#include "stdlib.h"

using namespace generic;

#define MAX_SENTENCE_SIZE 512

/*===============================================================
 *
 * tag
 *
 *==============================================================*/

void tag(const string sInputFile, const string sOutputFile, const string sFeaturePath) {
   cout << "Tagging started" << endl;
   int time_start = clock();
   CTagger tagger(sFeaturePath, false);
   CSentenceReader input_reader(sInputFile);
   CSentenceWriter output_writer(sOutputFile);
   CStringVector *input_sent = new CStringVector;
   CTwoStringVector *output_sent; 

   int nCount=0;
   
   const unsigned nBest = 1;
   output_sent = new CTwoStringVector[nBest];

   while( input_reader.readSegmentedSentence(input_sent) ) {
      TRACE("Sentence " << nCount);
      ++ nCount;
      //
      // Find decoder output
      //
      tagger.tag(input_sent, output_sent, nBest, NULL);
      //
      // Ouptut sent
      //
      for (int i=0; i<nBest; ++i)
         output_writer.writeSentence(output_sent+i, '/');
   }
   delete input_sent;
   delete [] output_sent;

   cout << "Tagging has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << endl;
}

/*===============================================================
 *
 * parse
 *
 *==============================================================*/

void parse(const string sInputFile, const string sOutputFile, const string sFeaturePath) {
   cout << "Parsing started" << endl;
   int time_start = clock();
   ostream *outs; if (sOutputFile=="") outs=&cout; else outs = new ofstream(sOutputFile.c_str()); 
   string sTaggerFeatureFile = sFeaturePath + "/tagger";
   string sParserFeatureFile = sFeaturePath + "/conparser";
   if (!FileExists(sTaggerFeatureFile))
      THROW("Tagger model does not exists. It should be put at model_path/tagger");
   if (!FileExists(sParserFeatureFile))
      THROW("Parser model does not exists. It should be put at model_path/conparser");
   CTagger tagger(sTaggerFeatureFile, false);
   CConParser conparser(sParserFeatureFile, false);
   CSentenceReader input_reader(sInputFile);
   CStringVector *input_sent = new CStringVector;
   CTwoStringVector *tagged_sent = new CTwoStringVector; 
   generic::CCFGTree *output_sent = new generic::CCFGTree; 

   unsigned nCount=0;
   
   CBitArray prunes(MAX_SENTENCE_SIZE);

   // If we read segmented sentence, we will ignore spaces from input. 
   while( input_reader.readSegmentedSentence(input_sent) ) {
      TRACE("Sentence " << nCount);
      ++ nCount;
      if ( input_sent->back()=="\n" ) {
         input_sent->pop_back();
      }
      tagger.tag(input_sent, tagged_sent, 1, NULL);
      conparser.parse(*tagged_sent, output_sent);
      // Ouptut sent
      (*outs) << output_sent->str_unbinarized() << endl;
   }
   delete input_sent;
   delete tagged_sent;
   delete output_sent;

   if (sOutputFile!="") delete outs;
   cout << "Parsing has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << endl;
}

/*===============================================================
 *
 * depparse
 *
 *==============================================================*/

void depparse(const string sInputFile, const string sOutputFile, const string sFeaturePath, bool bLabeled) {
   cout << "Parsing started" << endl;
   int time_start = clock();
   ostream *outs; if (sOutputFile=="") outs=&cout; else outs = new ofstream(sOutputFile.c_str()); 
   string sTaggerFeatureFile = sFeaturePath + "/tagger";
   string sParserFeatureFile = sFeaturePath + "/depparser";
   string sLabelerFeatureFile = sFeaturePath + "/deplabeler";
   if (!FileExists(sTaggerFeatureFile))
      THROW("Tagger model does not exists. It should be put at model_path/tagger");
   if (!FileExists(sParserFeatureFile))
      THROW("Parser model does not exists. It should be put at model_path/depparser");
   if (bLabeled && !FileExists(sLabelerFeatureFile))
      THROW("Labeler model does not exists. It should be put at model_path/deplabeler");
   cout << "[POS tagging module] "; cout.flush();
   CTagger tagger(sTaggerFeatureFile, false);
   cout << "[Parsing module] "; cout.flush();
   CDepParser depparser(sParserFeatureFile, false);
   CDepLabeler *deplabeler = 0;
   if (bLabeled) {
      cout << "[Labeling module] "; cout.flush();
      deplabeler = new CDepLabeler(sLabelerFeatureFile, false);
   }
   CSentenceReader input_reader(sInputFile);
   CStringVector input_sent;
   CTwoStringVector tagged_sent; 
   CDependencyTree parsed_sent; 
   CLabeledDependencyTree labeled_sent;

   unsigned nCount=0;
   
   CBitArray prunes(MAX_SENTENCE_SIZE);

   // If we read segmented sentence, we will ignore spaces from input. 
   while( input_reader.readSegmentedSentence(&input_sent) ) {
      TRACE("Sentence " << nCount);
      ++ nCount;
      if ( input_sent.back()=="\n" ) {
         input_sent.pop_back();
      }
      tagger.tag(&input_sent, &tagged_sent, 1, NULL);
      depparser.parse(tagged_sent, &parsed_sent, 1, NULL);
      if (bLabeled) {
         deplabeler->label(parsed_sent, &labeled_sent);
         // Ouptut sent
         (*outs) << labeled_sent;
      }
      else {
         (*outs) << parsed_sent;
      }
   }

   if (bLabeled) delete deplabeler;

   if (sOutputFile!="") delete outs;
   cout << "Parsing has finished successfully. Total time taken is: " << double(clock()-time_start)/CLOCKS_PER_SEC << endl;
}

/*===============================================================
 *
 * main
 *
 *==============================================================*/

int main(int argc, char* argv[]) {
   try {
      COptions options(argc, argv);
      CConfigurations configurations;
      configurations.defineConfiguration("o", "{t|d|u|c}", "output format; 't' pos-tagged format in sentences, 'd' refers to labeled dependency tree format, 'u' refers to unlabeled dependency tree format, and 'c' refers to constituent parse tree format", "d");

      if (options.args.size() < 2 || options.args.size() > 4) {
         cout << "\nUsage: " << argv[0] << " feature_path [input_file [output_file]]" << endl;
         cout << configurations.message() << endl;
         return 1;
      }
      string warning = configurations.loadConfigurations(options.opts);
      if (!warning.empty()) {
         cout << "Warning: " << warning << endl;
      }

      string sInputFile = options.args.size() > 2 ? options.args[2] : "";
      string sToFile = options.args.size() > 3 ? options.args[3] : "";
      string sOutFormat = configurations.getConfiguration("o");

      if (sOutFormat == "t")
          tag(sInputFile, sToFile, options.args[1]);
      if (sOutFormat == "c" )
          parse(sInputFile, sToFile, options.args[1]);
      if (sOutFormat == "d" )
          depparse(sInputFile, sToFile, options.args[1], true);
      if (sOutFormat == "u" )
          depparse(sInputFile, sToFile, options.args[1], false);
      return 0;
   } catch(const string&e) {cerr<<"Error: "<<e<<endl;return 1;}
}
