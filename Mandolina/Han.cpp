//�ѱ� Ŭ���� �޼ҵ�
#include <windows.h>

#include "Han.h"

#define HAN_MAX 2364
#define	MOUM	19

char* han1[]={"��","rk","��","rkr","�A","rkR","��","rks","��","rke","��","rkf","��","rkfr","��","rkfa","�E","rkfq","�F","rkft",
			"�G","rkfx","�H","rkfv","�I","rkfg","��","rka","��","rkq","��","rkqt","��","rkt","��","rkT","��","rkd","��","rkw",
			"��","rkc","��","rkx","��","rkv","��","rkg","��","ro","��","ror","��","ros","��","roe","��","roa","��","roq",
			"��","rot","��","roT","��","rod","��","ri","��","rir","��","ris","��","rif","��","rit","��","rid","��","rO",
			"��","rOs","��","rOf","��","rj","��","rjr","��","rjs","��","rje","��","rjf","��","rjfa","��","rja","��","rjq",
			"��","rjt","��","rjT","��","rjd","��","rjw","��","rjx","��","rjv","��","rjg","��","rp","��","rps","��","rpf",
			"��","rpa","��","rpq","��","rpt","��","rpT","��","rpd","��","ru","��","rur","��","rur","��","rus","��","rue",
			"��","ruf","��","rua","��","ruq","��","rut","��","ruT","��","rud","��","rux","��","rP","��","rPs","��","rPf",
			"��","rPq","��","rPt","��","rh","��","rhr","��","rhs","��","rhe","��","rhf","��","rhfa","��","rhft","��","rhfg",
			"��","rha","��","rhq","��","rht","��","rhd","��","rhw","��","rhk","��","rhkr","��","rhks","��","rhkf","��","rhkfa",
			"��","rhka","��","rhkq","��","rhkt","��","rhkd","��","rho","��","rhos","��","rhof","��","rhoq","��","rhoT","��","rhod",
			"��","rhl","��","rhlr","��","rhls","��","rhlf","��","rhla","��","rhlq","��","rhlt","��","rhld","��","ry","��","rys",
			"��","ryf","��","ryq","��","ryt","��","rn","��","rnr","��","rns","��","rne","��","rnf","��","rnfr","��","rnfa",
			"��","rnfg","��","rna","��","rnq","��","rnt","��","rnd","��","rnw","��","rnj","��","rnjr","��","rnjs","��","rnjf",
			"��","rnjT","��","rnjd","��","rnp","��","rnpt","��","rnl","��","rnlr","��","rnls","��","rnlf","��","rnla","��","rnlq",
			"��","rnlt","��","rb","��","rbs","��","rbf","��","rm","��","rmr","��","rms","��","rme","��","rmf","��","rmfr",
			"��","rma","��","rmq","��","rmt","��","rmd","��","rml","��","rl","��","rlr","��","rls","��","rle","��","rlf",
			"��","rlfa","��","rla","��","rlq","��","rlt","��","rld","��","rlw","��","rlv","��","Rk","��","Rkr","��","RkR",
			"��","Rks","��","Rkf","��","Rkfa","��","Rka","��","Rkq","��","Rkt","��","RkT","��","Rkd","��","Rkx","��","Ro",
			"��","Ror","��","Ros","��","Rof","��","Roa","��","Roq","��","Rot","��","RoT","��","Rod","��","Ri","��","Rir",
			"��","Rif","��","Rj","��","Rjr","��","RjR","��","Rjs","��","Rjf","��","Rja","��","Rjq","��","Rjt","��","RjT",
			"��","Rjd","��","Rp","��","Rpr","��","RpR","��","Rps","��","Rpa","��","Rpt","��","Rpd","��","Ru","��","Rus",
			"��","Ruf","��","Rut","��","RuT","��","Rux","��","RP","��","Rh","��","Rhr","��","Rhs","��","Rha","��","Rhsg",
			"��","Rhf","��","Rha","��","Rhq","��","Rht","��","Rhd","��","Rhw","��","Rhc","��","Rhk","��","Rhkr","��","Rhkf",
			"��","RhkT","��","Rhkd","��","Rho","��","Rhor","��","Rhod","��","Rhl","��","Rhls","��","RHlf","��","Rhla","��","Rhlq",
			"��","Rhld","��","Ry","��","Rn","��","Rnr","��","Rns","��","Rnf","��","Rnfg","��","Rna","��","Rnq","��","Rnt",
			"��","Rnd","��","Rnw","��","Rnj","��","Rnjf","��","RnjT","��","Rnjd","��","Rnp","��","Rnpr","��","Rnps","��","Rnpf",
			"��","Rnpa","��","Rnpq","��","RnpT","��","Rnl","��","Rnls","��","Rnlf","��","Rnla","��","Rnlq","��","Rb","��","Rm",
			"��","Rmr","��","Rms","��","Rmsg","��","Rmf","��","Rmfa","��","Rmfg","��","Rma","��","Rmq","��","Rmt","��","Rmd",
			"��","Rmx","��","Rl","��","Rlr","��","Rls","��","Rlf","��","Rla","��","Rlq","��","Rlt","��","Rld","��","sk",
			"��","skr","��","skR","��","sks","��","ske","��","skf","��","skfr","��","skfa","��","ska","��","skq","��","skt",
			"��","skT","��","skd","��","skw","��","skc","��","skx","��","skg","��","so","��","sor","��","sos","��","sof",
			"��","soa","��","soq","��","sot","��","soT","��","sod","��","si","��","sir","��","sis","��","sif","��","sia",
			"��","sid","��","sj","��","sjr","��","sjt","��","sjs","��","sjf","��","sjfa","��","sjfq","��","sja","��","sjq",
			"��","sjt","��","sjT","��","sjd","��","sjg","��","sp","��","spr","��","sps","��","spf","��","spa","��","spq",
			"��","spt","��","spT","��","spd","��","su","��","sur","��","sus","��","suf","��","sua","��","suq","��","suT",
			"��","sud","��","suz","��","sP","��","sPs","��","sh","��","shr","��","shs","��","shf","��","shfa","��","sha",
			"��","shq","��","sht","��","shd","��","shv","��","shg","��","shk","��","shks","��","shkf","��","shkT","��","shl",
			"��","shls","��","shlf","��","shla","��","shlq","��","shlt","��","sy","��","syr","��","sys","��","syf","��","syq",
			"��","syt","��","syd","��","sn","��","snr","��","sns","��","sne","��","snf","��","sna","��","snq","��","snt",
			"��","snd","��","snj","��","snjT","��","snp","��","snl","��","snls","��","snlf","��","snla","��","snlq","��","sb",
			"��","sbr","��","sbf","��","sba","��","sbq","��","sbd","��","sm","��","smr","��","sms","��","smf","��","smfr",
			"��","smfa","��","sma","��","smq","��","smt","��","smd","��","smw","��","smv","��","sml","��","smls","��","smlf",
			"��","sl","��","slr","��","sls","��","slf","��","slfa","��","sla","��","slq","��","slt","��","sld","��","slv",
			"��","ek","��","ekr","��","ekR","��","eks","��","eke","��","ekf","��","ekfr","��","ekfa","��","ekfq","��","ekfg",
			"��","eka","��","ekq","��","ekt","��","ekT","��","ekd","��","ekw","��","ekc","��","ekg","��","eo","��","eor",
			"��","eos","��","eof","��","eoa","��","eoq","��","eot","��","eoT","��","eod","��","ei","��","ej","��","ejr",
			"��","ejR","��","ejs","��","eje","��","ejf","��","ejfa","��","ejfq","��","eja","��","ejq","��","ejt","��","ejd",
			"��","ejc","��","ejv","��","ep","��","epr","��","eps","��","epf","��","epa","��","epq","��","ept","��","epT",
			"��","epd","��","eu","��","eus","��","euf","��","euT","��","eud","��","eP","��","ePs","��","eh","��","ehr",
			"��","ehs","��","ehe","��","ehf","��","ehfa","��","ehft","��","eha","��","ehq","��","eht","��","ehd","��","ehc",
			"��","ehx","��","ehk","��","ehks","��","ehkf","��","eho","��","ehoT","��","ehl","��","ehls","��","ehlf","��","ehla",
			"��","ehlq","��","ehlt","��","ey","��","en","��","enr","��","ens","��","enf","��","ena","��","enq","��","ent",
			"��","end","��","enj","��","enjT","��","enp","��","enpd","��","enl","��","enls","��","enlf","��","enlq","��","enlt",
			"��","enld","��","eb","��","ens","��","enf","��","ena","��","end","��","em","��","emr","��","ems","��","eme",
			"��","emf","��","emfa","��","ema","��","emq","��","emt","��","emd","��","eml","��","el","��","elr","��","els",
			"��","ele","��","elf","��","ela","��","elq","��","elt","��","elT","��","eld","��","elw","��","Ek","��","Ekr",
			"��","Eks","��","Ekf","��","Eka","��","Ekq","��","Ekt","��","EkT","��","Ekd","��","Ekg","��","Eo","��","Eor",
			"��","Eos","��","Eof","��","Eoa","��","Eoq","��","Eot","��","EoT","��","Eod","��","Ej","��","Ejr","��","Ejs",
			"��","Ejf","��","Ejfa","��","Ejfq","��","Eja","��","Ejq","��","Ejt","��","EjT","��","Ejd","��","Ejg","��","Ep",
			"��","Epr","��","Eps","��","Epf","��","Epa","��","Epq","��","Ept","��","EpT","��","Epd","��","Eu","��","EuT",
			"��","Eh","��","Ehr","��","Ehs","��","Ehf","��","Ehd","��","Ehk","��","Ehkf","��","Eho","��","Ehl","��","Ehls",
			"��","En","��","Enr","��","Ens","��","Enf","��","Enfg","��","Ena","��","End","��","Enp","��","Enl","��","Enls",
			"��","Enlf","��","Enla","��","Enlq","��","Enld","��","Em","��","Emr","��","Ems","��","Eme","��","Emf","��","Ema",
			"��","Emq","��","Emt","��","Eml","��","Emls","��","Emlf","��","Emla","��","Emlq","��","El","��","Els","��","Elf",
			"��","Ela","��","Elq","��","Elt","��","Eld","��","fk","��","fkr","��","fks","��","fkf","��","fka","��","fkq",
			"��","fkt","��","fkT","��","fkd","��","fkw","��","fkv","��","fkg","��","fo","��","for","��","fos","��","fof",
			"��","foa","��","foq","��","fot","��","foT","��","fod","��","fi","��","fir","��","fis","��","fit","��","fid",
			"��","fj","��","fjr","��","fjs","��","fjf","��","fja","��","fjq","��","fjt","��","fjT","��","fjd","��","fjg",
			"��","fp","��","fpr","��","fps","��","fpf","��","fpa","��","fpq","��","fpt","��","fpd","��","fu","��","fur",
			"��","fus","��","fuf","��","fua","��","fuq","��","fut","��","fuT","��","fud","��","fP","��","fPs","��","fPq",
			"��","fPt","��","fh","��","fhr","��","fhs","��","fhf","��","fha","��","fhq","��","fht","��","fhd","��","fhk",
			"��","fhks","��","fhkd","��","fhoT","��","fhl","��","fhls","��","fhlf","��","fhla","��","fhlq","��","fhlt","��","fhld",
			"��","fy","��","fys","��","fyf","��","fyq","��","fyt","��","fyd","��","fn","��","fnr","��","fns","��","fnf",
			"��","fna","��","fnq","��","fnt","��","fnd","��","fnj","��","fnjT","��","fnp","��","fnl","��","fnlr","��","fnls",
			"��","fnlf","��","fnla","��","fnlt","��","fnld","��","fb","��","fbr","��","fbs","��","fbf","��","fba","��","fbq",
			"��","fbt","��","fbd","��","fm","��","fmr","��","fms","��","fmf","��","fma","��","fmq","��","fmt","��","fmd",
			"��","fmw","��","fmx","��","fmv","��","fl","��","flr","��","fls","��","flf","��","fla","��","flq","��","flt",
			"��","fld","��","ak","��","akr","��","aks","��","aksg","��","ake","��","akf","��","akfr","��","aka","��","aka",
			"��","akq","��","akt","��","akd","��","akw","��","akx","��","akg","��","ao","��","aor","��","aos","��","aof","��","aoa",
			"��","aoq","��","aot","��","aoT","��","aod","��","aow","��","ai","��","air","��","aif","��","aid","��","aj","��","ajr",
			"��","ajs","��","ajf","��","ajfa","��","aja","��","ajq","��","ajt","��","ajd","��","ajw","��","ajg","��","ap","��","apr",
			"��","aps","��","apf","��","apa","��","apq","��","apt","��","apT","��","apd","��","au","��","aur","��","aus",
			"��","auf","��","aut","��","auT","��","aud","��","auc","��","aP","��","ah","��","ahr","��","ahrt","��","ahs","��","ahf",
			"��","ahfa","��","aha","��","ahq","��","aht","��","ahd","��","ahk","��","ahks","��","ahkT","��","ahkd","��","ahl","��","ahls",
			"��","ahlf","��","ahlq","��","ahlt","��","ahld","��","ay","��","ays","��","ayf","��","ayq","��","ayt","��","an","��","anr",
			"��","anR","��","ans","��","ane","��","anf","��","anfr","��","anfa","��","ana","��","anq","��","ant","��","and",
			"��","anx","��","ang","��","anj","��","anjs","��","anjf","��","anjq","��","anjt","��","anp","��","anl","��","anls","��","anlf",
			"��","ab","��","abs","��","abf","��","aba","��","abt","��","am","��","ams","��","amf","��","ama","��","amt","��","al",
			"��","alr","��","als","��","ale","��","alf","��","alfa","��","ala","��","alq","��","alt","��","alT","��","ald","��","alC",
			"��","alx","��","qk","��","qkr","��","qkR","��","qkrt","��","qks","��","qke","��","qkf","��","qkfr","��","qkfa",
			"��","qkfq","��","qka","��","qkq","��","qkt","��","qkd","��","qkx","��","qo","��","qor","��","qos","��","qof","��","qoa",
			"��","qoq","��","qot","��","qoT","��","qod","��","qox","��","qi","��","qir","��","qis","��","qiq","��","qj","��","qjr",
			"��","qjs","��","qje","��","qjf","��","qjfa","��","qja","��","qjq","��","qjt","��","qjd","��","qjw","��","qp","��","qpr",
			"��","qps","��","qpe","��","qpf","��","qpa","��","qpq","��","qpt","��","qpT","��","qpd","��","qu","��","qur",
			"��","qus","��","quf","��","quq","��","qut","��","quT","��","qud","��","qux","��","qP","��","qPs","��","qh","��","qhr",
			"��","qhR","��","qhs","��","qhf","��","qha","��","qhq","��","qht","��","qhd","��","qhk","��","qhks","��","qhkT","��","qho",
			"��","qhoT","��","qhl","��","qhlr","��","qhls","��","qhlf","��","qhla","��","qhlq","��","qy","��","qys","��","qn","��","qnr",
			"��","qns","��","qne","��","qnf","��","qnfr","��","qnfa","��","qna","��","qnq","��","qnt","��","qnd","��","qnx",
			"��","qnv","��","qnj","��","qnjf","��","qnjT","��","qnp","��","qnpf","��","qnpfr","��","qnl","��","qnlr","��","qnls","��","qnlf","��","qnld","��","qb",
			"��","qbs","��","qbf","��","qba","��","qbt","��","qbd","��","qm","��","qmr","��","qms","��","qmf","��","qma","��","qmq",
			"��","qmt","��","ql","��","qlr","��","qls","��","qlf","��","qlfa","��","qla","��","qlq","��","qlt","��","qld","��","qlw",
			"��","qlc","��","Qk","��","Qkr","��","Qks","��","Qkf","��","Qkfa","��","Qka","��","Qkq","��","Qkt","��","QkT",
			"��","Qkd","��","Qkg","��","Qo","��","Qor","��","Qos","��","Qof","��","Qoa","��","Qoq","��","Qot","��","QoT","��","Qod",
			"��","Qi","��","Qir","��","Qia","��","Qj","��","Qjr","��","Qjs","��","Qje","��","Qjf","��","Qja","��","Qjt","��","QjT",
			"��","Qjd","��","Qp","��","Qpd","��","Qu","��","Qur","��","Qua","��","Quq","��","Qut","��","QuT","��","Qud","��","Qh",
			"��","Qhr","��","Qhs","��","Qhf","��","Qha","��","Qhq","��","Qhd","��","Qhl","��","Qy","��","Qyd","��","Qn",
			"��","Qnr","��","Qns","��","Qnf","��","Qna","��","Qnt","��","Qnd","��","Qb","��","Qbd","��","Qm","��","Qms","��","Qmf",
			"��","Qma","��","Qmq","��","Ql","��","Qlr","��","Qls","��","Qlf","��","Qla","��","Qlq","��","Qlt","��","Qld","��","tk",
			"��","tkr","��","tkrt","��","tks","��","tke","��","tkf","��","tkfr","��","tkfa","��","tka","��","tkq","��","tkt","��","tkT",
			"��","tkd","��","tkx","��","to","��","tor","��","tos","��","tof","��","toa","��","toq","��","tot","��","toT",
			"��","tod","��","ti","��","tir","��","tis","��","tif","��","tia","��","tiq","��","tit","��","tid","��","tO",
			"��","tOs","��","tOf","��","tOa","��","tOd","��","tj","��","tjr","��","tjR","��","tjrt","��","tjs","��","tje",
			"��","tjf","��","tjfa","��","tjfq","��","tja","��","tjq","��","tjt","��","tjT","��","tjd","��","tjv","��","tp",
			"��","tpR","��","tps","��","tpf","��","tpa","��","tpq","��","tpt","��","tpT","��","tpd","��","tu","��","tur",
			"��","tus","��","tuf","��","tua","��","tuq","��","tut","��","tuT","��","tud","��","tP","��","tPs","��","tPf",
			"��","tPd","��","th","��","thr","��","thR","��","ths","��","thf","��","thfa","��","tha","��","thq","��","tht",
			"��","thd","��","thx","��","thk","��","thkr","��","thks","��","thkf","��","thkd","��","tho","��","thos","��","thof",
			"��","thoa","��","thot","��","thoT","��","thl","��","thls","��","thlf","��","thla","��","thlq","��","thlt","��","ty",
			"��","tyr","��","tys","��","tyf","��","tya","��","tyq","��","tyt","��","tyd","��","tn","��","tnr","��","tns",
			"��","tne","��","tnf","��","tna","��","tnq","��","tnt","��","tnd","��","tnc","��","tnx","��","tnv","��","tnj",
			"��","tnjT","��","tnp","��","tnpr","��","tnps","��","tnpf","��","tnpa","��","tnpd","��","tnl","��","tnlr","��","tnls",
			"��","tnlf","��","tnla","��","tnlq","��","tnlt","��","tnld","��","tb","��","tbr","��","tbf","��","tba","��","tbt",
			"��","tbd","��","tm","��","tmr","��","tms","��","tmf","��","tmfr","��","tma","��","tmq","��","tmt","��","tmd",
			"��","tl","��","tlr","��","tls","��","tle","��","tlf","��","tlfg","��","tla","��","tlq","��","tlt","��","tld",
			"��","tlv","��","Tk","��","Tkr","��","Tkrt","��","Tks","��","Tkf","��","Tka","��","Tkq","��","TkT","��","Tkd",
			"��","Tkg","��","To","��","Tor","��","Tos","��","Tof","��","Toa","��","Toq","��","ToT","��","Tod","��","Tid",
			"��","Tj","��","Tjr","��","Tjs","��","Tjf","��","Tjfa","��","Tja","��","Tjq","��","TjT","��","Tjd","��","Tp",
			"��","Tpr","��","Tps","��","Tpf","��","TPs","��","Th","��","Thr","��","Ths","��","The","��","Thf","��","Thfa",
			"��","Tha","��","Thq","��","Thd","��","Thk","��","Thkr","��","Thks","��","ThkT","��","Tho","��","ThoT","��","Thl",
			"��","Thls","��","Thlf","��","Thla","��","Thlq","��","Ty","��","Tn","��","Tnr","��","Tns","��","Tnf","��","Tna",
			"��","Tnq","��","Tnd","��","Tnj","��","TnT","��","Tnp","��","Tnl","��","Tnls","��","Tbd","��","Tm","��","Tmr",
			"��","Tms","��","Tmf","��","Tmfa","��","Tmfg","��","Tma","��","Tmq","��","Tml","��","Tmls","��","Tmlf","��","Tmla",
			"��","Tl","��","Tlr","��","Tls","��","Tlf","��","Tla","��","Tlq","��","Tlt","��","Tld","��","dk","��","dkr",
			"��","dks","��","dksw","��","dksg","��","dkf","��","dkfr","��","dkfa","��","dkfg","��","dka","��","dkq","��","dkt",
			"��","dkT","��","dkd","��","dkx","��","dkv","��","do","��","dor","��","dos","��","dof","��","doa","��","doq",
			"��","dot","��","doT","��","dod","��","di","��","dir","��","dis","��","dif","��","difq","��","dia","��","diq",
			"��","dit","��","did","��","dix","��","dig","��","dO","��","dOs","��","dOf","��","dOq","��","dj","��","djr",
			"��","djs","��","djsw","��","dje","��","djf","��","djfr","��","djfa","��","dja","��","djq","��","djqt","��","djt",
			"��","djT","��","djd","��","djw","��","djz","��","djv","��","dp","��","dpr","��","dps","��","dpf","��","dpa",
			"��","dpq","��","dpt","��","dpd","��","du","��","dur","��","duR","��","dus","��","duf","��","dufa","��","dufq",
			"��","dua","��","duq","��","duqt","��","dut","��","duT","��","dud","��","dux","��","duv","��","dug","��","dP",
			"��","dPs","��","dPf","��","dPa","��","dPq","��","dPt","��","dPT","��","dh","��","dhr","��","dhs","��","dhf",
			"��","dhfr","��","dhfa","��","dhft","��","dhfg","��","dha","��","dhq","��","dht","��","dhd","��","dhc","��","dhk",
			"��","dhkr","��","dhks","��","dhkf","��","dhka","��","dhkq","��","dhkt","��","dhkT","��","dhkd","��","dho","��","dhor",
			"��","dhos","��","dhoa","��","dhot","��","dhod","��","dhl","��","dhlr","��","dhls","��","dhlf","��","dhla","��","dhlq",
			"��","dhlt","��","dhld","��","dy","��","dyr","��","dys","��","dyf","��","dya","��","dyq","��","dyr","��","dyd",
			"��","dn","��","dnr","��","dns","��","dnf","��","dnfr","��","dnfa","��","dna","��","dnq","��","dnt","��","dnd",
			"��","dnj","��","dnjr","��","dnjs","��","dnjf","��","dnja","��","dnjq","��","dnjT","��","dnjd","��","dnp","��","dnpr",
			"��","dnps","��","dnpf","��","dnpa","��","dnpq","��","dnpd","��","dnl","��","dnlr","��","dnls","��","dnlf","��","dnla",
			"��","dnlq","��","dnlt","��","dnld","��","db","��","dbr","��","dbs","��","dbf","��","dba","��","dbq","��","dbt",
			"��","dbd","��","dbc","��","dm","��","dmr","��","dms","��","dmf","��","dmfv","��","dma","��","dnq","��","dmt",
			"��","dmd","��","dmw","��","dmc","��","dmz","��","dmx","��","dmv","��","dmg","��","dml","��","dmls","��","dmlf",
			"��","dmla","��","dmlt","��","dl","��","dlr","��","dls","��","dlf","��","dlfr","��","dlfa","��","dlfg","��","dla",
			"��","dlq","��","dlt","��","dlT","��","dld","��","dlw","��","dlv","��","wk","��","wkr","��","wks","��","wksg",
			"��","wke","��","wkf","��","wkfa","��","wka","��","wkq","��","wkt","��","wkT","��","wkd","��","wkw","��","wo",
			"��","wor","��","wos","��","wof","��","woa","��","woq","��","wot","��","woT","��","wod","��","wi","��","wir",
			"��","wis","��","wisg","��","wif","��","wia","��","wid","��","wO","��","wOs","��","wOf","��","wj","��","wjr",
			"��","wjs","��","wjf","��","wjfa","��","wja","��","wjq","��","wjt","��","wjd","��","wjw","��","wp","��","wpr",
			"��","wps","��","wpf","��","wpa","��","wpq","��","wpt","��","wpd","��","wu","��","wus","��","wuf","��","wua",
			"��","wuq","��","wuT","��","wud","��","wP","��","wh","��","whr","��","whs","��","whf","��","whfa","��","wha",
			"��","whq","��","wht","��","whd","��","whw","��","whc","��","whg","��","whk","��","whkr","��","whkf","��","whkq",
			"��","whkt","��","whkd","��","who","��","whoT","��","whod","��","whl","��","whls","��","whlf","��","whla","��","whlq",
			"��","whlt","��","whld","��","wy","��","wyr","��","wys","��","wyd","��","wn","��","wnr","��","wns","��","wnf",
			"��","wnfr","��","wnfa","��","wna","��","wnq","��","wnt","��","wnd","��","wnj","��","wnjT","��","wnp","��","wnl",
			"��","wnlr","��","wnls","��","wnlf","��","wnla","��","wnlq","��","wnlt","��","wb","��","wbs","��","wbf","��","wba",
			"��","wm","��","wmr","��","wms","��","wmf","��","wma","��","wmq","��","wmt","��","wmd","��","wl","��","wlr",
			"��","wls","��","wle","��","wlf","��","wlfa","��","wla","��","wlq","��","wlt","¡","wld","¢","wlw","£","wlx",
			"¤","wlv","¥","Wk","¦","Wkr","§","Wks","¨","Wksg","©","Wkf","ª","Wkfq","«","Wka","¬","Wkq","­","Wkt",
			"®","WkT","¯","Wkd","°","Wo","±","Wor","²","Wos","³","Wof","´","Woa","µ","Woq","¶","Wot","·","WoT",
			"¸","Wod","¹","Wi","º","Wis","»","Wid","¼","Wj","½","Wjr","¾","Wjs","¿","Wjf","��","Wja","��","Wjq",
			"��","Wjt","��","WjT","��","Wjd","��","Wp","��","Wpd","��","Wu","��","WuT","��","Wh","��","Whr","��","Whs",
			"��","Whf","��","Wha","��","Whq","��","Wht","��","Whd","��","Whc","��","Whk","��","Whkr","��","Whkf","��","WhkT",
			"��","Who","��","WhoT","��","Whl","��","Whls","��","Whlf","��","Whla","��","Whlq","��","Wyd","��","Wn","��","Wnr",
			"��","Wns","��","Wnf","��","Wna","��","Wnq","��","Wnd","��","Wnj","��","WnjT","��","Wnjd","��","Wnl","��","Wb",
			"��","Wm","��","Wma","��","Wmt","��","Wmd","��","Wl","��","Wlr","��","Wls","��","Wlf","��","Wla","��","Wlq",
			"��","Wld","��","Wlw","��","Wlg","��","ck","��","ckr","��","cks","��","cksg","��","ckf","��","cka","��","ckq",
			"��","ckt","á","ckT","â","ckd","ã","ckw","ä","co","å","cor","æ","cos","ç","cof","è","coa","é","coq",
			"ê","cot","ë","coT","ì","cod","í","ci","î","cis","ï","cisg","ð","cif","ñ","cia","ò","cid","ó","cj",
			"ô","cjr","õ","cjs","ö","cjf","÷","cja","ø","cjq","ù","cjt","ú","cjT","û","cjd","ü","cp","ý","cpr",
			"þ","cps","ÿ","cpf","��","cpa","��","cpq","��","cpt","��","cpd","��","cu","��","cus","��","cuT","��","cP",
			"��","cPs","��","cPd","��","ch","��","chr","��","chs","��","chf","��","cha","��","chq","��","cht","��","cha",
			"��","chk","��","chks","��","chkf","��","chkd","��","chl","��","chls","��","chlf","��","chla","��","chlq","��","chlt",
			"��","chld","��","cy","��","cya","�h","cyq","��","cn","��","cnr","��","cns","��","cnf","��","cna","��","cnq",
			"��","cnt","��","cnd","��","cnj","��","cnjT","��","cnp","��","cnps","�X","cnpt","��","cnl","��","cnls","��","cnlf",
			"��","cnla","��","cnlq","��","cnlt","��","cnld","��","cb","��","cbs","��","cbf","��","cba","��","cbd","��","cm",
			"��","cmr","��","cms","��","cmf","��","cma","��","cmq","��","cmt","��","cmd","ġ","cl","Ģ","clr","ģ","cls",
			"Ĥ","cle","ĥ","clf","Ħ","clfr","ħ","cla","Ĩ","clq","ĩ","clt","Ī","cld","ī","zk","Ĭ","zkr","ĭ","zks",
			"Į","zkf","į","zka","İ","zkq","ı","zkt","Ĳ","zkd","ĳ","zo","Ĵ","zor","ĵ","zos","Ķ","zof","ķ","zoa",
			"ĸ","zoq","Ĺ","zot","ĺ","zoT","Ļ","zod","ļ","zi","Ľ","zir","ľ","zid","Ŀ","zj","��","zjr","��","zjs",
			"��","zje","��","zjf","��","zja","��","zjq","��","zjt","��","zjT","��","zjd","��","zp","��","zp","��","zps",
			"��","zpf","��","zpa","��","zpq","��","zpt","��","zpd","��","zu","��","zus","��","zuf","��","zua","��","zuq",
			"��","zut","��","zuT","��","zud","��","zP","��","zh","��","zhr","��","zhs","��","zhf","��","zha","��","zhq",
			"��","zht","��","zhd","��","zhk","��","zhkr","��","zhks","��","zhkf","��","zhka","��","zhkd","��","zho","��","zhod",
			"��","zhl","��","zhlf","��","zy","��","zn","��","znr","��","zns","��","znf","��","zna","��","znq","��","znt",
			"��","znd","��","znj","��","znjs","��","znjf","��","znjd","��","znp","��","znpd","��","znl","��","znlr","��","znls",
			"��","znlf","š","znla","Ţ","znlq","ţ","znlt","Ť","znld","ť","zb","Ŧ","zbs","ŧ","zbf","Ũ","zba","ũ","zm",
			"Ū","zmr","ū","zms","Ŭ","zmf","ŭ","zma","Ů","zmq","ů","zmd","Ű","zl","ű","zlr","Ų","zls","ų","zlf",
			"Ŵ","zla","ŵ","zlq","Ŷ","zlt","ŷ","zld","Ÿ","xk","Ź","xkr","ź","xks","Ż","xkf","ż","xkfr","Ž","xka",
			"ž","xkq","ſ","xkt","��","xkT","��","xkd","��","xo","��","xor","��","xos","��","xof","��","xoa","��","xoq",
			"��","xot","��","xoT","��","xod","��","xi","��","xid","��","xj","��","xjr","��","xjs","��","xjf","��","xjfa",
			"��","xja","��","xjq","��","xjt","��","xjT","��","xjd","��","xp","��","xpr","��","xps","��","xpf","��","xpa",
			"��","xpq","��","xpt","��","xpd","��","xu","��","xus","��","xuT","��","xP","��","xPs","��","xh","��","xhr",
			"��","xhs","��","xhf","��","xha","��","xhq","��","xht","��","xhd","��","xhv","��","xhk","��","xhks","��","xho",
			"��","xhl","��","xhls","��","xhlt","��","xhld","��","xy","��","xn","��","xnr","��","xns","��","xnf","��","xna",
			"��","xnq","��","xnt","��","xnd","��","xnj","��","xnjT","ơ","xnp","Ƣ","xnl","ƣ","xnlr","Ƥ","xnls","ƥ","xnlf",
			"Ʀ","xnla","Ƨ","xnlq","ƨ","xnld","Ʃ","xb","ƪ","xbs","ƫ","xb","Ƭ","xba","ƭ","xbd","Ʈ","xm","Ư","xmr",
			"ư","xms","Ʊ","xme","Ʋ","xmf","Ƴ","xmfa","ƴ","xma","Ƶ","xmq","ƶ","xmt","Ʒ","xml","Ƹ","xls","ƹ","xmlf",
			"ƺ","xmla","ƻ","xmlq","Ƽ","xl","ƽ","xlr","ƾ","xls","ƿ","xlf","��","xla","��","xlq","��","xlt","��","xld",
			"��","vk","��","vkr","��","vkR","��","vks","��","vkf","��","vkfa","��","vka","��","vkq","��","vkt","��","vkT",
			"��","vkd","��","vkx","��","vko","��","vor","��","vos","��","vof","��","voa","��","voq","��","vot","��","voT",
			"��","vod","��","vi","��","vir","��","vj","��","vjr","��","vjs","��","vjf","��","vja","��","vjq","��","vjt",
			"��","vjT","��","vjd","��","vp","��","vpr","��","vps","��","vpf","��","vpa","��","vpq","��","vpt","��","vpd",
			"��","vu","��","vus","��","vuf","��","vua","��","vuq","��","vuT","��","vud","��","vP","��","vPf","��","vPq",
			"��","vPt","��","vh","��","vhr","��","vhs","��","vhf","��","vha","��","vhq","��","vht","��","vhd","ǡ","vhk",
			"Ǣ","vhkd","ǣ","vhl","Ǥ","vhls","ǥ","vy","Ǧ","vys","ǧ","vyf","Ǩ","vyq","ǩ","vyt","Ǫ","vn","ǫ","vnr",
			"Ǭ","vns","ǭ","vne","Ǯ","vnf","ǯ","vnfa","ǰ","vna","Ǳ","vnq","ǲ","vnt","ǳ","vnd","Ǵ","vnj","ǵ","vnjd",
			"Ƕ","vnl","Ƿ","vnls","Ǹ","vnlf","ǹ","vnla","Ǻ","vnlt","ǻ","vb","Ǽ","vbs","ǽ","vbf","Ǿ","vba","ǿ","vbt",
			"��","vbd","��","vm","��","vms","��","vmf","��","vma","��","vmq","��","vmt","��","vl","��","vlr","��","vls",
			"��","vlf","��","vla","��","vlq","��","vlt","��","vld","��","gk","��","gkr","��","gks","��","gkf","��","gkfx",
			"��","gka","��","gkq","��","gkt","��","gkd","��","go","��","gor","��","gos","��","gof","��","goa","��","goq",
			"��","got","��","goT","��","god","�d","gog","��","gi","��","gid","��","gj","��","gjr","��","gjs","��","gjf",
			"��","gjfa","��","gja","��","gjq","��","gjt","��","gjd","��","gp","��","gpr","��","gps","��","gpf","��","gpa",
			"��","gpq","��","gpt","��","gpd","��","gu","��","gur","��","gus","��","guf","��","gua","��","guq","��","gut",
			"��","guT","��","gud","��","gP","��","gPs","ȡ","gPf","Ȣ","gPq","ȣ","gh","Ȥ","ghr","ȥ","ghs","Ȧ","ghf",
			"ȧ","ghfx","Ȩ","gha","ȩ","ghq","Ȫ","ght","ȫ","ghd","Ȭ","ghx","ȭ","ghk","Ȯ","ghkr","ȯ","ghks","Ȱ","ghkf",
			"ȱ","ghkt","Ȳ","ghkd","ȳ","gho","ȴ","ghor","ȵ","ghos","ȶ","ghot","ȷ","ghod","ȸ","ghl","ȹ","ghlr","Ⱥ","ghls",
			"Ȼ","ghlf","ȼ","ghlq","Ƚ","ghlt","Ⱦ","ghld","ȿ","gy","��","gys","��","gyf","��","gyq","��","gyt","��","gn",
			"��","gnr","��","gns","��","gnf","��","gnfx","��","gna","��","gnt","��","gnd","��","gnj","��","gnjs","��","gnjf",
			"��","gnja","��","gnjd","��","gnp","��","gnpr","��","gnps","��","gnpf","��","gnpd","��","gnl","��","gnlr","��","gnls",
			"��","gnlf","��","gnla","��","gnlq","��","gnlt","��","gnld","��","gb","��","gbr","��","gbs","��","gbf","��","gba",
			"��","gbt","��","gbd","��","gm","��","gmr","��","gms","��","gmsg","��","gme","��","gmf","��","gmfr","��","gma",
			"��","gmq","��","gmt","��","gmd","��","gmx","��","gml","��","gmls","��","gmlf","��","gmla","��","gmlq","��","gmld",
			"��","gl","��","glr","��","gls","��","glf","��","gla","��","glq","��","glt","��","gld"};

char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��",
					"��","��","��","��","��","��","��","��","��","��","��","��","��","��"};

CHan::CHan()
{
	//�˻� �ӵ� ����� ���� ��������
	int mp=0;
	char* check_text[19]={"��","��","��","��","��","��","��","��","��","��","��","��","��","¥","��","ī","Ÿ","��","��"};
	for(unsigned int i=0; i<HAN_MAX; i++)
	{
		if(strcmp(han1[i*2], check_text[mp])==0)
		{
			mark[mp]=i;
			++mp;
			if(mp>=19)break;
		}
	}
	mark[19]=HAN_MAX;
}

unsigned int CHan::FindMark(char eng)
{
	for(unsigned int j=0; j<19; ++j)
		if(eng==jamo[j])return j;

	return 0;
}

char* CHan::EngToHan(char* text, char* han_area)
{
	unsigned int pp=0;
	unsigned int begin[STR_MAX], end[STR_MAX];

	//�Է� �ޱ�
	strcpy(han, "");

	//�ѱۿ� ��ġ �� �Ǵ� �빮�ڸ� �ҹ��ڷ� �ٲ�
	for(unsigned int i=0; i<strlen(text); i++)
	{
		if(han_area[i]==0)continue;
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;
	}

	//����, ���� �ѱ� �ƴ� ���� Ȯ��(���� �� �κ��� ����Ʈ�� �ξ ��ȯ�� ������ �ǰ� ��)
	for(unsigned int i=1; i<strlen(text); i++)
	{
		//�������� �������� ����
		for(int j=0; j<33; ++j)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					++pp;
					i++;	//���� ������ ��츦 �����ؼ� ������ �ѱ�(���� ������ ���� �о� ���ڰ� �Ǵ� ���� �����Ƿ� ��: �޻�(O) �ΤӤ���(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=(unsigned int)strlen(text)-1;

	//����Ʈ ����(�ѱ��� �κи� �ؼ�)
	for(unsigned int i=0; i<pp; i++)
	{
		char buffer[10];
		int bp=0;
		BOOL head = TRUE;	//�� �κ����� ���� �޺κ����� �˾Ƴ�(�߶󳻾� �ϴ� �κ� ��: 1@��2$ -> 1@�� head=TRUE�� ����, 2$�� head=FALSE�� ���¿��� ����
		for(unsigned int j=begin[i]; j<=end[i]; ++j)
		{
			if(han_area[j] && (text[j]>='a' && text[j]<='z' || text[j]>='A' && text[j]<='Z'))
			{
				buffer[bp]=text[j];
				++bp;
				head=FALSE;
			}
			else
			{
				if(head)++begin[i];
					else end[i]=j-1;
			}
		}
		buffer[bp]=NULL;

		//������ �ѱ۷� ���� �� ��ġ�ϴ� ���� ã��
		while(strlen(buffer)>1)
		{
			BOOL find=FALSE;
			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];

			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					find=TRUE;
					break;
				}

			//ã�� �� ���� �� ������ �ٿ��� ã�ƺ�
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				--end[i];
			}
			else break;
		}

		//���� ���ϴ� �ѱ����� ǥ��
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//��ȯ
	unsigned int pp2=0;
	for(unsigned int i=0; i<strlen(text); i++)
	{
		while(begin[pp2]==9999)++pp2;

		//�ѱ��� �κ�
		if(pp2<pp && i==begin[pp2])
		{
			char buffer[10];
			int bp=0;
			for(unsigned int j=begin[pp2]; j<=end[pp2]; ++j)
			{
				buffer[bp]=text[j];
				++bp;
			}
			buffer[bp]=NULL;

			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];
			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					strcat(han, han1[j*2]);
					break;
				}

			i=end[pp2];
			++pp2;
		}
		//�ƴ� �κ�
		else
		{			
			//�ѱ� �϶��� ������ ���� �������� ǥ��
			if(han_area[i]!=0 && (text[i]>='a' && text[i]<='z' || text[i]>='A' && text[i]<='Z'))
			{
				for(int j=0; j<33; ++j)
				{
					if(text[i]==jamo[j])
					{
						strcat(han, han_jamo[j]);
						break;
					}
				}
			}
			//�ᱹ ���� �� ���� �״��
			else
			{
				char tmp[2];
				tmp[0]=text[i];
				tmp[1]=NULL;
				strcat(han, tmp);
			}			
		}
	}

	han[strlen(han)]=NULL;
	return han;
}