#include <stdio.h>
#include <stdlib.h>
#include "ocl_data_structures.h"
void add_nodes(struct Node **head, int num)
{
        //THIS FUNCTION ADD NODES TO THE ARRAY OF LINKED LISTS
        struct Node *aux_node=(struct Node *)malloc(sizeof(struct Node));
        aux_node->num = num;
        aux_node->next=NULL;
        if(*head==NULL)
        {
                *head = aux_node;
        }
        else
        {
                struct Node *p;
                p = *head;
                while(p->next!=NULL)
                {
                        p = p->next;

                }
                p->next=aux_node;
        }

}
void disp_ad_matrix(struct Node **admat,int n_reg)
{
        //THIS FUNCTION DISPLAYS TO THE ARRAY OF LINKED LISTS
        int i=0;
        for(int k=0; k<n_reg; k++)
        {

                struct Node *current;
                for(current=*admat; current!=NULL; current = current->next)
                {
                        printf("%d -->\t",current->num);

                }
                admat++;
                printf("%s","NULL");
                printf("\n");
        }

}
void get_ad_matrix(struct Node **admat,int n_reg,char filename[256])
{
        //THIS MODULE GETS THE ADJACENCY MATRIX USING THE 
        // GRAPH CONECTIONS IN THE GRAPH FILE 
        FILE *input;
        int k=0,j=0,num,Nreg;
        if((input = fopen(filename,"r"))==NULL)
        {
                printf("FAILED TO OPEN FILE\n");
        }
        fscanf(input,"%d%*[^\n]\n",&Nreg);
        if(n_reg!=Nreg)
        {
                printf("WRONG FILE: NUMBRE OF REGIONS ARE DIFFERENT FROM THE GRAPH FILE\n");
        }
        for(int i=0;i<n_reg; i++)
        {
                admat[i] = NULL;
        }
        fscanf(input,"%d",&num);
        while(!feof(input))
        {
                if(num!=-1)
                {
                        j=num;
                        add_nodes(&admat[k],j);
                        j++;
                }
                else
                {
                        k++;
                }
               fscanf(input,"%d",&num);

        }
        fclose(input);

}
