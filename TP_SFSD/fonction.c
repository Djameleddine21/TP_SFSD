#include "fonction.h"
///La machine abstraite

void Ouvrir(FILE **f,char nomfichier[20],char mode) ///Ouvrir le fichier
{
    int i=0;
    if (mode=='A' ||mode =='a') ///si le fichier existe deja
    {
        *f=fopen(nomfichier,"rb+");
    }
    else
    {
        if (mode=='N' ||mode =='n') ///si le fichier est nouveau
        {
            *f=fopen(nomfichier,"wb+");
            Aff_entete(*f,1,&i);
            Aff_entete(*f,2,&i);
            Aff_entete(*f,3,&i);
            Aff_entete(*f,4,&i);
            Aff_entete_nom(*f,nomfichier);
        }
        else  ///si le mode n'est pas reconue
        {
            exit(0);
        }
    }
}

void Fermer(FILE **f) ///fermer le fichier
{
    fclose(*f);
}

void LireDir(FILE *f,int i,bloc *buf) ///lire dans le buffer le contenu de i'eme bloc
{
    fseek(f,sizeof(entete)+sizeof(char)*20+sizeof(bloc)*(i-1),SEEK_SET); ///positionement de cursor
    fread(buf,sizeof(bloc),1,f);
    cou++;
}

void EcrireDir(FILE *f,int i,bloc *buf) ///Ecrire le contenu de buf dans le i'eme bloc dans fichier f
{
    fseek(f,sizeof(entete)+20*sizeof(char)+(i-1)*sizeof(bloc),SEEK_SET); ///positionement de cursor
    fwrite(buf,sizeof(bloc),1,f);
    cou++;
}

int Entete(FILE *f,int i) ///retourne la valeur de la i'eme valeur d'entete
{
    int r;
    switch(i)
        {
        case 1: ///le nombre de bloc
            fseek(f,0,SEEK_SET);
            fread(&r,sizeof(int),1,f);
            break;
        case 2: ///le nombre d'articles
            fseek(f,sizeof(int),SEEK_SET);
            fread(&r,sizeof(int),1,f);
            break;
        case 3: ///le nombre de caracteres supprimees
            fseek(f,2*sizeof(int),SEEK_SET);
            fread(&r,sizeof(int),1,f);
            break;
        case 4: ///le nombre de caracteres inserees
            fseek(f,3*sizeof(int),SEEK_SET);
            fread(&r,sizeof(int),1,f);
            break;
        default: ///rien
            break;
        }
        cou++;
        return r;
}

void Aff_entete(FILE *f,int i,int *val ) ///affecte la valeur val a la i'eme valeur dans l'entete
{
    switch(i)
        {
        case 1: ///le nombre de blocs
            fseek(f,0,SEEK_SET);
            fwrite(val,sizeof(int),1,f);
            break;
        case 2: ///le nombre d'articles
            fseek(f,sizeof(int),SEEK_SET);
            fwrite(val,sizeof(int),1,f);
            break;
        case 3: ///le nombre de caracteres supprimees
            fseek(f,2*sizeof(int),SEEK_SET);
            fwrite(val,sizeof(int),1,f);
            break;
        case 4: ///le nombre de caracteres inserees
            fseek(f,3*sizeof(int),SEEK_SET);
            fwrite(val,sizeof(int),1,f);
            break;
        default: ///rien
            break;
        }
        cou++;
}

void Aff_entete_nom(FILE *f,char nomfichier[20]) ///affecter un nom a un fichier
{
    cou++;
    fseek(f,4*sizeof(int),SEEK_SET); ///positionement de cursor
    fwrite(nomfichier,20*sizeof(char),1,f);
}

void nom(FILE *f,char nomfichier[20]) ///retourner le nom d'un fichier
{
    cou++;
    fseek(f,4*sizeof(int),SEEK_SET); ///positionement de cursor
    fread(nomfichier,20*sizeof(char),1,f);
}

void recherche (FILE *f ,int cle ,int *i ,int *j ,booleen *trouv) ///recherche la cle dans f
{
    bloc buf1,buf2;
    int bi,bs,nb_bloc=Entete(f,1);
    booleen stop=faux ;
    *i=1 ; *j=0 ; *trouv=faux ;
    if (nb_bloc==0) ///si le fichier est vide
    {
        *trouv=faux;
        *i=0;
        *j=0;
    }
    else
    {
        LireDir(f,1,&buf1);
        if(cle <= buf1.cle_max) ///la cle peut existe dans la premier bloc
        {
            *i=1;
            rech(f,&buf1,cle,j,trouv);
        }
        else
        {
            *i=nb_bloc;
            LireDir(f,*i,&buf1);
            if(cle > buf1.cle_max) /// la cle est doit insere soient dans le dernier bloc
            {
                if (buf1.t_libre==0) ///le dernier bloc est remplis
                {
                    *i=*i+1;  /// donc il faut ajouter un nouveau bloc
                    *j=0;
                    *trouv=faux;
                }
                else ///le dernier bloc contient d'espace vide
                {
                    rech(f,&buf1,cle,j,trouv);  /// on recherche la position j dans le bloc i
                }
            }
            else ///on fait un recherhce dichotomique entre les blocs
            {
                LireDir(f,(*i)-1,&buf2);
                if((cle > buf2.cle_max)&&(cle <buf1.cle_max)) ///la cle peut etre dans le dernier bloc
                {
                   rech(f,&buf1,cle,j,trouv); /// on recherche la position j dans le bloc i
                }
                else
                {
                    bi=1 ; bs=*i ;
                    while((bi <= bs)&&(!stop)) /// on fait la recherche dichotomique
                    {
                        *i=(bs+bi)/2 ;
                        LireDir(f,*i,&buf1);
                        LireDir(f,(*i)-1,&buf2);
                        if ((cle > buf2.cle_max)&&(cle <= buf1.cle_max))
                        {
                            stop=vrai ;
                            rech(f,&buf1,cle,j,trouv);
                        }
                        else
                        {
                            if (cle < buf2.cle_max)
                            {
                                bs=*i ;
                            }
                            else
                            {
                                bi=*i ;
                            }
                        }
                    }
                }
            }
        }
    }
}

void rech(FILE *f,bloc *buf,int cle,int *j,booleen *trouv) ///recherche la position j du cle dans le bloc (dans buf)
{
    int n,k,nb_bloc=Entete(f,1);
    booleen stop=faux ;
    char s[11010],s1[5];
    bloc buf1;
    if(buf->nb_c==0) /// on fait la recherche a partir de la position 0
    {
        *j=0;
    }
    else ///si le premier article ne commence pas de l'indice 0 car il y a des caracteres chevauchés du bloc précédent
    {
        *j=buf->nb_c;
    }
    strcpy(s,buf->tab); /// on copie le buffer dans S
    n=buf->n_bloc+1 ;
    if (n<=nb_bloc) /// c'est pas le dernier bloc
    {
        LireDir(f,n,&buf1);
        k=10000;
        for(n=0;n<buf1.nb_c;n++)  /// on recupere les char qui sont en cheval avec le bloc suivant
        {
            s[k]=buf1.tab[n];
            k++;
        }
        s[k+1]='\0';
    }
    *trouv=faux;
    *j=0;
    while((!stop)&&(!*trouv))
    {
        k=0 ;
        for (n=*j;n<=*j+3;n++)
        {
            s1[k]=s[n+4];
            k++;
        }
        s1[4]='\0';
        n=atoi(s1); ///On recupere la cle
        if (n==cle) /// i.e on a trouvé la cle
        {
            *trouv=vrai;
        }
        else
        {
            if (n>cle) /// c'est la position ou il faut inserer la cle
            {
                stop=vrai;
            }
            else
            {
                ///On recupere la taille du enreg
                k=0 ;
                memset(s1,'\0',sizeof(s1));
                for(n=*j;n<=*j+2;n++)
                {
                    s1[k]=buf->tab[n];
                    k++ ;
                }
                s1[3]='\0';
                ///on recupere position du enreg suivant
                *j=*j+9+atoi(s1);
                if( *j >=10000-buf->t_libre || *j>10000)/// on verifier qu'on a pas depassé la taille du buffer ou la taille libre
                {
                    stop=vrai;
                }
            }
        }
    }
}

void  insertion(enreg e,FILE *f) ///L'insertion d'une cle
{
    int i,j,m,n,k,t,cle,nb_bloc=Entete(f,1);  int value;
    char s1[2000],s2[2000];
    booleen trouv,b=faux;
    bloc buf;
    cle=atoi(e.cle); /// on récupère la cle comme un entier pour facilicte la comparaison
    strcpy(s1,e.taille); /// On copier l'enreg dans une chaine
    s1[3]='\0';
    strcat(s1,"0\0");
    strcat(s1,e.cle);
    s1[8]='\0';
    strcat(s1,"0\0");
    strcat(s1,e.info);
    t=atoi(e.taille)+9;  /// La taille de tout l'enreg
    if (nb_bloc==0)  /// Cas de fichier vide
    {
        i=1;
        Aff_entete(f,1,&i); /// On mis a jour le nombre de bloc
        Aff_entete(f,2,&i); /// On mis a jour le nombre des articles
        i=0;
        Aff_entete(f,3,&i);  /// On mis a jour le nombre des char supprimer
        i=atoi(e.taille)+9;
        Aff_entete(f,4,&i); /// On mis a jour le nombre des chars insérer
        buf.n_bloc=1;  /// le numero du bloc
        buf.cle_max=cle ;   /// la cle max
        buf.nb_c=0;  /// le nombre des char en cheval
        strcpy(buf.tab,s1);
        buf.t_libre=10000-strlen(s1);  /// on mis a jour la taille libre
        EcrireDir(f,1,&buf);
    }
    else  /// le fichier contient des blocs
    {
        recherche(f,cle,&i,&j,&trouv);  /// On cherche la cle
        if(i<=nb_bloc) /// la cle est doit insérer dans un bloc existe deja
        {
            LireDir(f,i,&buf);
            if (trouv)  /// On verifier c'est la clé est supprimé logiquement
            {
                if (j+3 <= 9999)  /// le champ vide est dans le bloc i
                {
                    if(buf.tab[j+3]=='1')  /// i.e la cle est supprimé
                    {
                        buf.tab[j+3]='0';  /// on mis a jour le champ vide
                        EcrireDir(f,i,&buf);
                        m=Entete(f,2)+1;  /// on mis a jour le nombre des articles
                        Aff_entete(f,2,&m);
                        n=0;
                        for(m=j;m<=j+2;m++)
                        {
                            s1[n]=buf.tab[m];
                            n++;
                        }
                        s1[3]='\0';
                        m=Entete(f,3)-9-atoi(s1) ; /// on mis a jour le nombre des chars supprimer
                        Aff_entete(f,3,&m);
                    }
                }
                else  /// i.e le champ vide est le bloc i+1
                {
                    j=9999-j;
                    switch(j)
                    {
                        case 0 : j=2 ;
                                break;
                        case 1 : j=1 ;
                                break ;
                        case 2 : j=0 ;
                                break;
                    }
                    LireDir(f,i+1,&buf);
                    if (buf.tab[j]=='1')
                    {
                        buf.tab[j]='0';
                        EcrireDir(f,i+1,&buf);
                        m=Entete(f,2)+1;  /// on mis a jour le nombre des articles
                        Aff_entete(f,2,&m);
                        j++;
                        n=0;
                        for(m=j;m<=j+2;m++)
                        {
                            s1[n]=buf.tab[m];
                            n++;
                        }
                        s1[3]='\0';
                        m=Entete(f,3)-9-atoi(s1) ; /// on mis a jour le nombre des chars supprimer
                        Aff_entete(f,3,&m);
                    }
                }
            }
            else  ///i.e Non(trouv) => cle n'existe pas deja
            {
                n=nb_bloc; ///on recupère le nombre du bloc
                m=i;
                while (m<=nb_bloc)  /// On fait des decalage a partir du bloc i jusqu'a dernier bloc
                {
                    LireDir(f,m,&buf);
                    decalage(&buf,j,s1,s2,&b);
                    j=0 ;
                    EcrireDir(f,m,&buf);
                    if(b)  /// le b indique c'est la chaine s2 est contient des char qu'il faut inserer dans le bloc suivant
                    {
                        if(m<nb_bloc)
                        {
                            memset(s1,0,strlen(s1));
                            strcpy(s1,s2);
                        }
                        else  ///i.e m = nb_bloc donc c'est le dernier bloc ... il faut d'ajouté un nouveau bloc
                        {
                            memset(buf.tab,0,sizeof(buf.tab));
                            buf.n_bloc=nb_bloc+1;
                            buf.t_libre=10000-strlen(s2);
                            strcpy(buf.tab,s2);
                            n=0 ;
                            k=0 ;
                            while((n<8)&&(k<strlen(s2)))/// on verifier c'est s2 contient un enreg
                            {
                                if((s2[k]<=57)&&(s2[k]>=48)) ///Le code ASCI des chiffres
                                {
                                    n++ ;
                                }
                                else
                                {
                                    n=0;
                                }
                                k++;
                            }
                            buf.nb_c=k-8; /// le nombre de char en cheval
                            /// On mis a jour la valeur de la cle_max
                            if (buf.nb_c < strlen(buf.tab)) /// Le nouveau bloc conteint des enreg
                            {
                                n=buf.nb_c ; /// on parcour s2 jusqu'a dernier enreg
                                k=0; n=0;  memset(s1,0,strlen(s1));
                                 while(k<strlen(s2))/// on verifier c'est s2 contient un enreg
                                    {
                                        if((s2[k]<=57)&&(s2[k]>=48)) n++; else  n=0;  ///Le code ASCI des chiffres
                                        if(n==8)
                                            {
                                              strncpy(s1,s2+(k-4)*sizeof(char),3);
                                              value=atoi(s1); ///On recupere la cle
                                              if (value>buf.cle_max) buf.cle_max=value;
                                              strncpy(s1,s2+(k-8)*sizeof(char),3);
                                              k+=atoi(s1);
                                              n=0;
                                            }
                                           else  k++;
                                    }
                            }
                            n=nb_bloc+1;
                            Aff_entete(f,1,&n); ///On mis a jour le nombre du bloc
                            EcrireDir(f,n,&buf);
                        }
                    }
                    m++;
                }
                    n=Entete(f,2)+1; ///On mis a jour le nombre des articles
                    Aff_entete(f,2,&n);
                    n=Entete(f,4)+t; ///On mis a jour le nombre des char insérer
                    Aff_entete(f,4,&n);
            }
        }
        else /// il faut d'ajouter un nouveau bloc puisque la recherhce retourne i > nb_bloc i.e tout les bloc son remplis
        {
            Aff_entete(f,1,&i); /// on mis a jour le nombre des blocs
            i=Entete(f,2)+1;
            Aff_entete(f,2,&i); /// on mis a jour le nombre des articles
            i=atoi(e.taille)+9;
            Aff_entete(f,4,&i); /// on mis a jour le nombre des char inserer
            buf.n_bloc=i;
            buf.cle_max=cle ; /// on mis a jour la cle max
            buf.nb_c=0;      /// on mis a jour le nombre des chars en cheval
            strcpy(buf.tab,s1);
            buf.t_libre=10000-strlen(s1);  /// on mis a jour la taille libre dans le bloc
            EcrireDir(f,i,&buf);
        }
    }
}

void decalage(bloc *buf,int j, char s1[],char s2[],booleen *b) ///on fait un decalage a partir de la position j i.e un inser s1 a partir de la position j
{
    char s[11010];
    int i,k=0,n=0,t=strlen(s1);
    booleen stop=faux;
    memset(s,'\0',sizeof(s));
    n=0;
    for(k=0;k<j;k++) /// on recupere les premiers char dans S jusqu'a la position j
    {
        s[n]=buf->tab[k];
        n++;
    }
    s[j]='\0';
    strcat(s,s1); /// on concatene s avec s1 qui represente la chaine qu'il faut inserer a partir de la position j
    k=strlen(s);
    for(i=j;i<=9999;i++)  /// on copier le reste du bloc dans S
    {
        s[k]=buf->tab[i];
        k++;
    }
    for (i=0;i<=9999;i++) /// on copier s dans buf.tab
    {
        buf->tab[i]=s[i];
    }
    if(strlen(s)>10000)  /// dans ce cas il y a des char en plus qui ne sont pas insérer dans buf.tab
    {
        k=0;
        memset(s2,0,strlen(s2));
        *b=vrai;
        for(i=10000;i<strlen(s);i++) /// on copier le reste des char dans s2
        {
            s2[k]=s[i];
            k++;
        }
    }
    else
    {
        memset(s2,0,strlen(s2));
        *b=faux;
    }
    ///On verifie la taille libre dans buf
    if(*b==vrai)
    {
        buf->t_libre=0;
    }
    else
    {
        buf->t_libre=buf->t_libre - t;
        if( buf->t_libre< 0)
        {
            buf->t_libre=0;
        }
    }
    ///On mis à jour  le nombre des chars qui sont chevauchement dans buf
    n=0 ;
    k=0 ;
    while((n<=8) &&(k<strlen(buf->tab)))
    {
        if((buf->tab[k] <= 57)&&(buf->tab[k]>= 48)) ///Le code ASCI des chiffres
        {
            n++ ;
        }
        else
        {
            n=0;
        }
        k++;
    }
    buf->nb_c=k-8;
    ///On recuper la position du dernier cle dans le bloc pour modifier la cla max ...
    i=0;
    while(i<=9999 && !stop)
    {
        j=i;
        n=0;
        for(k=i;k<=i+2;k++)
        {
            s1[n]=buf->tab[k];
            n++;
        }
        s1[3]='\0';
        n=0;
        for(k=i+4;k<=i+7;k++)
        {
            s1[n]=buf->tab[k];
            n++;
        }
        s1[4]='\0';
        if (atoi(s1)>=buf->cle_max)
        {
            stop=vrai;
        }
        else
        {
            i=i+atoi(s1)+9 ; ///On recupere la taille
        }
    }
    n=0;
    for(k=j+4;k<=j+7;k++)
    {
        s1[n]=buf->tab[k];
        n++;
    }
    s1[4]='\0';
    buf->cle_max=atoi(s1); ///On mis a jour la cle_max
}

void affiche(FILE *f) ///afficher le contenu de fichier bloc par bloc
{
	int sort,i,j,k,n,m,cpt,nb_bloc=Entete(f,1),nb_article=Entete(f,2);
	bloc buf;
	enreg e;
	i=1;
	j=0;
	char s[1008];
	cpt=1;
	if(nb_bloc!=0) ///le fichier n'est pas vide
	{
		while(i <=nb_bloc && cpt<=nb_article) ///boucle pour le parcours des blocs
		{
			if(j>9999) ///si on a deppasse la taille maximal de bloc
			{
				i++;
				if(i>Entete(f,1)) ///si on a depasse le nombre des blocs
					sort=1;
				else ///lecture d'un nouvelle bloc
				{
					LireDir(f,i,&buf);
					sort=0;
					if (buf.nb_c==0) ///si le premier article de bloc est a l'indice 0
						j=0;
					else ///s'il existe des caracteres en chevauchement de bloc precedent traités dans l'iteration precedente
						j=buf.nb_c;
				}
			}
			else ///si on est toujours dans le meme bloc mais faut reinitialiser le buffer en cas de lecture de bloc prochaine s'il existait un chevauchement
        	{
            	sort=0;
				LireDir(f,i,&buf);
        	}
			k=0;
			n=j;
			while(k<=1007 && sort==0) ///la recuperation d'un article
			{
				if((n<=9999) && (buf.tab[n]!='\0'))
				{
					s[k]=buf.tab[n];
					k++;
					n++;
				}
                else
				{
					if(i<nb_bloc)
					{
						LireDir(f,i+1,&buf);
						n=0;
					}
					else
					{
						sort=1;
					}
				}
			}
			if(k!=0) ///si on a recupere un article
			{
				s[k]='\0';
				for (m = 0; m < 3;m++) ///recuperation de la taille
				{
					e.taille[m]=s[m];
				}
				e.taille[3]='\0';
				if(s[3]=='1') ///verification si l'article est vide
					j=j+9+atoi(e.taille);
				else
				{
					for(m=0;m<4;m++) ///recuperation de la cle
					{
						e.cle[m]=s[m+4];
					}
					e.cle[4]='\0';
					for(m=0;m<atoi(e.taille);m++) ///recuperation de l'info de l'article
					{
						e.info[m]=s[m+9];
					}
					e.info[atoi(e.taille)]='\0';
					printf("Le numero de bloc: %d\nLa cle: %s\nLa taille: %s\nL'article: %s\n",i,e.cle,e.taille,e.info);
					cpt++;
					j=j+9+atoi(e.taille); ///passe au prochaine article
				}
			}
		}
	}
	else
		printf("Le fichier est vide\n");
}


void afficheBloc(FILE *f,int i) ///afficher le contenu de bloc article par article
{
	int sort,cpt,stop,j,k=1,n,m,nb_bloc,nb_article=Entete(f,2);
	bloc buf;
	enreg e;
	char s[1008];
	nb_bloc=Entete(f,1);
	if(k!=0 && i<=nb_bloc) ///si le fichier n'est pas vide et le numero de bloc demandé existe
	{
		LireDir(f,i,&buf);
		j=buf.nb_c-1;
		stop=0;
		sort=0;
		cpt=0;
		while(stop==0 && cpt<nb_article && e.cle<buf.cle_max) ///la boucle pour recuperer les articles
		{
			if(j>9999) ///si le bloc est fini
			{
				stop=1;
			}
			else ///s'il existe toujours des articles a lire
        	{
            	sort=0;
				LireDir(f,i,&buf);
        	}
			k=0;
			n=j;
			while(k<=1007 && sort==0) ///la boucle pour recuperer un article
			{
				if((n<=9999) && (buf.tab[n]!='\0'))
				{
					s[k]=buf.tab[n];
					k++;
					n++;
				}
			else
				{
					if(i<nb_bloc)
					{
						LireDir(f,i+1,&buf);
						n=0;
					}
					else
					{
						sort=1;
					}
				}
			}
			if(k!=0) ///si on a lit un article
			{
				s[k]='\0';
				for (m = 0; m < 3;m++) ///pour recuperer la taille
				{
					e.taille[m]=s[m];
				}
				e.taille[3]='\0';
				if(s[3]==1) ///pour verifier si l'article est supprime
					j=j+9+atoi(e.taille);
				else
				{
					for(m=0;m<4;m++) ///pour recuperer la cle
					{
						e.cle[m]=s[m+4];
					}
					e.cle[4]='\0';
					for(m=0;m<atoi(e.taille);m++) ///pour recuperer l'info de l'article
					{
						e.info[m]=s[m+9];
					}
					e.info[atoi(e.taille)]='\0';
					printf("Le numero de bloc: %d\nLa cle: %s\nLa taille: %s\nL'article: %s\n",i,e.cle,e.taille,e.info);
					j=j+9+atoi(e.taille);
					cpt++;
				}
			}
		}
	}
	else
		printf("Le bloc n'existe pas\n");
}

void Suppression(FILE *f,int cle) ///supprimer l'article de cle
{
	int i,j,k,n;
	booleen trouv;
	bloc buf;
	char taille[4];
	char vide='0';
	recherche (f ,cle ,&i ,&j ,&trouv);
	if(trouv) ///si l'article existe
	{
		n=j;
		LireDir(f,i,&buf);
		///recuperation de la taille et la position vide
		for (k = 0; k < 4;k++)
		{
			if (n<=9999 && k<3)
				taille[k]=buf.tab[n];
			else
			{
				if(k<3)
				{
					n=0;
					i++;
					LireDir(f,i,&buf);
					taille[k]=buf.tab[n];
				}
				else
				{
					taille[k]='\0';
					if(n<=9999)
						vide=buf.tab[n];
					else
					{
						n=0;
						i++;
						LireDir(f,i,&buf);
						vide=buf.tab[n];
					}
				}
			}
			n++;
		}
		if(vide=='0') ///mis a jour de l'indice
		{
			buf.tab[n-1]='1';
			EcrireDir(f,i,&buf);
			i=Entete(f,2)-1;
			Aff_entete(f,2,&i);
			if(i==0)
                Aff_entete(f,1,&i);
			i=Entete(f,3)+atoi(taille)+9;
			Aff_entete(f,3,&i);
		}
	}
}


void afficheEntete(FILE *f) ///afficher le contenu de l'entete
{
    char s[20];
    nom(f,s);
    printf("Le nom de fichier: %s\nLe nombre de blocs est: %d\nLe nombre d'article est: %d\nLe nombre de characteres supprimees est: %d\nLe nombre de characteres inserees est: %d\n",s,Entete(f,1),Entete(f,2),Entete(f,3),Entete(f,4));
}

void reorganisation(FILE **f,int aut) ///reorganiser le fichier si auto=1 donc c automatique
{
	FILE *f1;
	int sort,i,j,k,n,m,cpt,nb_supp=Entete(*f,3),nb_bloc=Entete(*f,1),nb_ins=Entete(*f,2);
	bloc buf;
	enreg e;
	i=1;
	j=0;
	char s[1008];
	cpt=1;
	if(nb_supp!=0)
    {
        if(nb_bloc!=0)
        {
            Ouvrir(&f1,"tmp.bin",'N');
            while(i <=nb_bloc && cpt<=nb_ins) ///recuperation des article meme principe avec affiche
            {
                if(j>9999)
                {
                    i++;
                    if(i>nb_bloc)
                        sort=1;
                    else
                    {
                        LireDir(*f,i,&buf);
                        sort=0;
                        if (buf.nb_c==0)
                            j=0;
                        else
                            j=buf.nb_c-1;
                    }
                }
                else
                {
                    sort=0;
                    LireDir(*f,i,&buf);
                }
                k=0;
                n=j;
                while(k<=1007 && sort==0)
                {
                    if(n<=9999)
                    {
                        s[k]=buf.tab[n];
                        k++;
                        n++;
                    }
                    else
                    {
                        if(i<nb_bloc)
                        {
                            LireDir(*f,i+1,&buf);
                            n=0;
                        }
                        else
                        {
                            sort=1;
                        }
                    }
                }
                if(k!=0)
                {
                    s[k]='\0';
                    for (m = 0; m < 3;m++)
                    {
                        e.taille[m]=s[m];
                    }
                    e.taille[3]='\0';
                    if(s[3]=='1')
                        j=j+9+atoi(e.taille);
                    else
                    {
                        for(m=0;m<4;m++)
                        {
                            e.cle[m]=s[m+4];
                        }
                        e.cle[4]='\0';
                        for(m=0;m<atoi(e.taille);m++)
                        {
                            e.info[m]=s[m+9];
                        }
                        e.info[atoi(e.taille)]='\0';
                        strcpy(e.vide,"0");
                        insertion(e,f1);
                        cpt++;
                        j=j+9+atoi(e.taille);
                    }
                }
            }
            nom(*f,s);
            Fermer(f);
            remove(s);
            if(aut==0) ///si la reorganisation est manual
            {
                printf("Donnez le nom de nouvelle fichier: ");
                scanf("%s",s);
            }
            Fermer(&f1);
            rename("tmp.bin",s);
            Ouvrir(f,s,'A');
            Aff_entete_nom(*f,s);
        }
        else
        {
            printf("Le fichier est vide\n");
            i=0;
            Aff_entete(*f,3,&i);
            Aff_entete(*f,4,&i);
        }
    }
    else
        printf("Le fichier n'a pas besoin de reorganisation\n");
}

void chargementinit(FILE *f,int n) ///charger le fichier f par n article aleatoires
{
    int i,j,c,t=0;
    char s[2];
    enreg article;
    for (i=1;i<=n;i++)
    {
        j=rand()%999+1;
        t=t+j+9 ;
        //strcpy(s,"");
        memset(s,'\0',sizeof(s));
        itoa(j/100,s,10);
        article.taille[0]=s[0];
        memset(s,'\0',sizeof(s));
        itoa((j/10)%10,s,10);
        article.taille[1]=s[0];
        memset(s,'\0',sizeof(s));
        itoa(j%10,s,10);
        article.taille[2]=s[0];
        article.taille[3]='\0';
        j=rand()%9999+1;
        memset(s,'\0',sizeof(s));
        itoa(j/1000,s,10);
        article.cle[0]=s[0];
        memset(s,'\0',sizeof(s));
        itoa((j/100)%10,s,10);
        article.cle[1]=s[0];
        memset(s,'\0',sizeof(s));
        itoa((j/10)%10,s,10);
        article.cle[2]=s[0];
        memset(s,'\0',sizeof(s));
        itoa(j%10,s,10);
        article.cle[3]=s[0];
        article.cle[4]='\0';
        strcpy(article.c,"0");
        strcpy(article.vide,"0");
        for(j=0;j<atoi(article.taille);j++) {c=(rand()%(122-97)+1)+97; article.info[j]=c;}
        article.info[atoi(article.taille)]='\0';
        insertion(article,f);
    }
}


//////////////////////////////// *** LES MODULES DE L'AFFICHAGE *** //////////////////////////////////////////////
//////////////////////////////////////// *** MODULE POUR  GO TO UNE PLACE SPECIFIE *** //////////////////////////////////////////
void gotoxy(int xpos, int ypos)
{
        COORD scrn;
        HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);

        scrn.X = xpos-1;
        scrn.Y = ypos;
        SetConsoleCursorPosition(hOuput, scrn);
}
///////////////////////////////// **** MODULE DES COULEURS *** /////////////////////////////////////////////////////////
void color(int t,int f)
{
        HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(H,f*16+t);
}
//////////////////////////////// *** MODULE POUR STOPER LE SYSTEME *** /////////////////////////////////////////////////
int delay(int x) // faire un puase au system pendant c millierseconds
{
	clock_t t1,t2;
	float tmp;
	t1=clock();
	do
	{
		t2=clock();
		tmp=(float)(t2-t1)*1000/CLOCKS_PER_SEC ;

	} while (tmp < x);
	return 0;

}
//////////////////////////// *** MODULE POUR EFFACER L'ECRAN *** ///////////////////////////////////////////////////////////////
void clr_pro(int t,int lig )
{
	int i,j ;
	for (i=110;i>=0;i--)
	{
		if (i == 54) break;
		for (j=lig;j<40;j++)
		{
			gotoxy(i,j);

			color(0,0);
			printf(" ");
			gotoxy(109-i,38-j);
			printf(" ");
		}
		delay(t);
	}
	gotoxy(109,39);

	return ;
}
///////////////////////////////////////// *** MODULE POUR AFFICHE LE CHARGEMENT *** /////////////////////////////////////////////
void chargement()
{
	int h ;
	color(0,15);
	gotoxy(45,20);
	printf("Chargement en cours");
	for (h=0;h<100;h++)
	{
		gotoxy(6+h,22);
		color(9,9);
		printf("  ");
		color(0,15);
		gotoxy(53,21);
		printf("%d%c",h+1,37);
		gotoxy(130,39);
		color(15,15);
		printf(" ");
		if ((h >60) && (h <80)) delay(80);
		else delay(20);
	}
	gotoxy(38,30);
	color(15,0);
	printf("press any key to continue ...");
	getch();
	clr_pro(0,0);
	return ;
}
/////////////////////////////////////////// *** MODULE POUR L'ECRIRE *** ////////////////////////////////////////////////////
int ecrire(char ss[],int tm) // ecrier une chaine de caractaire avec un delay entre les caracteres
{
	int uu ;
	for (uu=0;uu<strlen(ss);uu++)
	{
		delay(tm);
		printf("%c",ss[uu]);

	}
	return 0;
}
/////////////////////////////////////////// *** MODULE POUR ECRIRE ESI *** ///////////////////////////////////////////////////
void esi (int x, int y,int fond,int couleur,int t)
{
	color(couleur,couleur);
	// E
	gotoxy(x,y);
	printf("*******");
	delay(t);
	gotoxy(x,y+1);
	printf("*******");
	delay(t);
	gotoxy(x,y+2);
	printf("**");
	delay(t);
	gotoxy(x,y+3);
	printf("*****");
	delay(t);
	gotoxy(x,y+4);
	printf("*****");
	delay(t);
	gotoxy(x,y+5);
	printf("**");
	delay(t);
	gotoxy(x,y+6);
	printf("*******");
	delay(t);
	gotoxy(x,y+7);
	printf("*******");
	delay(t);
	// S
	gotoxy(x+8,y+7);
	printf("*******");
	delay(t);
	gotoxy(x+8,y+6);
	printf("*******");
	delay(t);
	gotoxy(x+8+5,y+5);
	printf("**");
	delay(t);
	gotoxy(x+8,y+4);
	printf("*******");
	delay(t);
	gotoxy(x+8,y+3);
	printf("*******");
	delay(t);
	gotoxy(x+8,y+2);
	printf("**");
	delay(t);
	gotoxy(x+8,y+1);
	printf("*******");
	delay(t);
	gotoxy(x+8,y);
	printf("*******");
	delay(t);
	// I
	int ii=0,jj=0;
	for (jj=0;jj<6;jj++)
	{
        for (ii = 0;ii<3;ii++)
        {
            gotoxy(x+16+ii,y+7-jj);
            printf("*");
            delay(t);
        }
	}
	// tete
	for (ii=0;ii<3;ii++)
	{
	switch (ii)
	{
	case 0 : color(4,7);
			break;
	case 1 : color(10,7);
			break;
	case 2 : color(9,7);
			break ;
	}
	gotoxy(x+16,y+1);
	printf("%c",_1);
	delay(t);
	gotoxy(x+17,y+1);
	printf("%c",_);
	delay(t);
	gotoxy(x+18,y+1);
	printf("%c",_3);
	delay(t);
	gotoxy(x+16,y);
	printf("%c",_7);
	delay(t);
	gotoxy(x+17,y);
	printf("%c",_);
	delay(t);
	gotoxy(x+18,y);
	printf("%c",_9);
	delay(t);
	}
	color(9,7);
    gotoxy(x+21,y+1);
    ecrire("Ecole nationale\n",20);
    gotoxy(x+21,y+3);
    ecrire("Superieure\n",20);
    gotoxy(x+21,y+5);
    ecrire("d'Informatique",20);
	gotoxy(110,39);
	printf("\n\n\n");
	return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void textbackground(int bcolor)
{
     WORD wColor;
     //We will need this handle to get the current background attribute
     HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
     CONSOLE_SCREEN_BUFFER_INFO csbi;

     //We use csbi for the wAttributes word.
     if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
     {
        //make a color form the current backgound and our forground color
          wColor = (csbi.wAttributes & 0xF) | ((bcolor  << 4 ) & 0xF0);
          SetConsoleTextAttribute(hStdOut, wColor);

     }
}
int wherey()
{
        CONSOLE_SCREEN_BUFFER_INFO coninfo;

        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
        return coninfo.dwCursorPosition.Y;
}

int afficher(int i) //concerne affichage de menu
{
    color(0,0);
    entete_menu();
    char * tab[]={"1-> Rechercher un article a partir de sa cle\n","2-> Inserer  un article\n","3-> Supprimer un article logiquement\n","4-> Afficher l'entete du fichier\n","5-> Afficher le contenu d'un bloc donne\n","6-> Afficher le contenu de tout le fichier\n","7-> Reorganiser le fichier\n","8-> Chargement initial\n","9-> Quitter\n\n"};
    int j=0;
    while (j<9)
    {
        if (j+1 == i)  textbackground(12);
        else textbackground(0);
        {
            gotoxy(10,wherey());
            printf("%s\n",tab[j]);
        }
        j++;
    }
    textbackground(0);
   int choix=i;
   return choix;
}
void menu(FILE *f)
{
    int sauv=1,stop=0,i=0,j=0,done=0,n;
    char cle[5];
    char s[2];
    booleen trouv=faux;
    enreg e;
    color(15,0);
    entete_menu();
    afficher(sauv);
    while(stop==0)
    {
        char c;
        c=getch();
        if (c =='1')
         sauv=afficher(1);
        else if (c=='2')
         sauv=afficher(2);
        else if (c=='3')
         sauv=afficher(3);
         else if (c=='4')
         sauv=afficher(4);
         else if (c=='5')
         sauv=afficher(5);
         else if (c=='6')
         sauv=afficher(6);
         else if (c=='7')
         sauv=afficher(7);
         else if (c=='8')
         sauv=afficher(8);
        else if (c=='9')
         sauv=afficher(9);
        else if (c==72)
            {if ((sauv-1)>=1) sauv=afficher(sauv-1);else sauv=afficher(NB_FONCTIONALITES);}
        else if (c==80)
            {if ((sauv+1)<=NB_FONCTIONALITES) sauv=afficher(sauv+1);else sauv=afficher(1);}
        else if (c==13)

        switch(sauv)
        {
            case 1 :  // Recherche
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0) ;
                    done=0;
                    while(done==0)
                    {
                        done=1;
                        printf("introduire la cle : ");
                        scanf("%s",cle);
                        for(i=0;i<strlen(cle);i++)
                        {
                            if(cle[i]<48 || cle[i]>57)
                                done=0;
                        }
                    }
                    recherche(f,atoi(cle),&i,&j,&trouv);
                    if(trouv)
                    {
                        printf("La cle est dans le bloc %d a l'indice %d \n",i,j);
                    }
                    else
                    {
                        printf("Cle non trouvee !\n");
                    }
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(1);
                    break;
            case 2 :  // Insertion
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    done=0;
                    while(done==0)
                    {
                        printf("Introduire la cle (sur 4 chiffres) : ");
                        scanf("%s",e.cle);
                        if(strlen(e.cle)==4) done=1 ;
                    }
                    done=0;
                    printf("introduire la l'article : ");
                    scanf("%s",e.info);
                    n=strlen(e.info);
                    strcpy(s,"");
                    itoa(n/100,s,10);
                    e.taille[0]=s[0];
                    strcpy(s,"");
                    itoa((n/10)%10,s,10);
                    e.taille[1]=s[0];
                    strcpy(s,"");
                    itoa(n%10,s,10);
                    e.taille[2]=s[0];
                    e.taille[3]='\0';
                    strcpy(e.vide,"0\0");
                    strcpy(e.c,"0\0");
                    insertion(e,f);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(2);
                    break;
            case 3 :  // Suppresion
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    printf("Introduire la cle : ");
                    scanf("%s",cle);
                    Suppression(f,atoi(cle));
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(3);
                    break;
            case 4 :  // Afficher l'entete
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    afficheEntete(f);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(4);
                    break;
            case 5 :  // Afficher le contenu d'un bloc
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    printf("Introduire le numero du bloc : ");
                    scanf("%d",&i);
                    if(i!=0)
                    {
                        afficheBloc(f,i);
                    }
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(5);
                    break;
            case 6 :  // Afficher le contenu de tout le fichier
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    affiche(f);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(6);
                    break;
            case 7 :  // Reorganiser le fichier
                    cou=0;
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    reorganisation(&f,0);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(7);
                    break;
            case 8 :
                    system("cls");
                    entete_menu();
                    textbackground(0);
                    cou=0;
                    printf("introduire le nombre d'articles : ");
                    scanf("%d",&i);
                    chargementinit(f,i);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    system("pause");
                    sauv=afficher(8);
                    break ;
            case 9 :  // Quitter
                    cou=0;
                    reorganisation(&f,1);
                    printf("\n\n");
                    color(15,6);
                    printf("le cout est %d acces au disque",cou);
                    color(15,0);
                    printf("\n\n");
                    exit(1);
            default :exit(0);
        }
    }
}
void entete_menu()
{
    system("cls");
    printf("\t\t");
    color(15,1);
    printf("-----------------------------------------------------------------------\n");
    color(0,0);
    printf("\t\t");
    color(15,1);
    printf("|                 TP1_ALGO ....... Realise par : SEBBAGH & MOUMNA     |\n");
    color(0,0);
    printf("\t\t");
    color(15,1);
    printf("-----------------------------------------------------------------------\n\n\n");
}
void accueil()
{
    cou=0;
    FILE *f;
    char s[20];
    char m;
  /*  system("title TP ALGO ");
    system("color F0");
    esi(35,5,8,0,10);
    chargement();*/
    entete_menu();
    textbackground(0);
    printf("Introduire le nom du fichier avec l'extension '.bin' : ");
    scanf("%s",s);
    fflush(stdin);
    printf("choisir le mode que vous vlouez (A ou a) pour ancien et (N ou n) pour nouveau : ");
    m=getc(stdin);
    Ouvrir(&f,s,m);
    menu(f);
    Fermer(&f);
}
