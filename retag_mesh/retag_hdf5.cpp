#include <iostream>
#include <sstream>
#include <iomanip> // for setprecision
#include <limits> // for min/max values
#include <assert.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <stdio.h>

#include "MBInterface.hpp"
#include "MBCore.hpp"
#include "MBTagConventions.hpp"
#include "MBTagConventions.hpp"
#include "MBRange.hpp"
#include "moab/Range.hpp"
#include "moab/Core.hpp"
#include "moab/Skinner.hpp"
#include "moab/GeomTopoTool.hpp"

MBInterface *MBI();

struct tag_info {
  moab::Tag mbtag;
  moab::TagType mbtype;
  std::string mbtagname;
};

int main(int argc, char **argv) 
{
  std::string input_name;
  MBErrorCode rval;
  MBEntityHandle input_mesh_set;

  input_name = argv[1]; // set the filename
  rval = MBI()->create_meshset( MESHSET_SET, input_mesh_set ); // create entity set
  if(rval != MB_SUCCESS)
    std::cout << "failed to create meshset" << std::endl;
  rval = MBI()->load_file( input_name.c_str(), &input_mesh_set ); // open the mesh
  if(rval != MB_SUCCESS)
    std::cout << "failed to load file" << std::endl;
  
  // vars to store the list of tag names and 
  std::vector<moab::Tag> tag_list;
  std::vector<moab::Tag>::const_iterator tl_it;
  std::vector<std::string> tag_names;
  std::vector<moab::TagType> tag_types;

  rval = MBI()->tag_get_tags(tag_list); // get the list of tag handles

  moab::Tag dag_tag;

  for ( tl_it = tag_list.begin() ; tl_it != tag_list.end() ; ++tl_it ) // loop 
    {
      moab::TagType tmp_tagtype;
      moab::Tag mbtag;
      std::string tmp_tagname;

      mbtag = *tl_it; // a given tag handle
      rval = MBI()->tag_get_name(mbtag,tmp_tagname); // get the name of the tag handle
      rval = MBI()->tag_get_type(mbtag,tmp_tagtype); // the type of the tag handle
      if("DAGMC_TAG_NAME"==tmp_tagname)
	dag_tag = mbtag;
      tag_types.push_back(tmp_tagtype);
      tag_names.push_back(tmp_tagname);
      std::cout << tmp_tagname << " " << tmp_tagtype << std::endl;
    }


  MBTag category_tag;
  rval = MBI()->tag_get_handle( CATEGORY_TAG_NAME, 32, MB_TYPE_OPAQUE, category_tag );
  if(rval!=MB_SUCCESS)
    {
      std::cout << "Fail 1" << std::endl;
      exit(1);
    }
  char group_category[CATEGORY_TAG_SIZE];
  std::fill(group_category, group_category+CATEGORY_TAG_SIZE, '\0');
  sprintf(group_category, "%s", "Group");
  printf(group_category,"%s");
  const void* const group_val[] = {&group_category};
  MBRange groups;
  rval = MBI()->get_entities_by_type_and_tag(0, MBENTITYSET, &category_tag, 
                                           group_val, 1, groups);
  if(rval!=MB_SUCCESS)
    {
      std::cout << "Fail 2" << std::endl;
      exit(1);
    }

  int nmat = 1;

  MBTag name_tag;
  rval = MBI()->tag_get_handle( NAME_TAG_NAME, NAME_TAG_SIZE, MB_TYPE_OPAQUE, name_tag );
  for( MBRange::iterator i = groups.begin(); i != groups.end(); ++i )
    {
      MBEntityHandle grp = *i;
      const void* p;
      int ignored;
      rval = MBI()->tag_get_by_ptr( name_tag, &grp, 1, &p, &ignored );
      if( MB_SUCCESS != rval ) return rval;
      const char* grpname = static_cast<const char*>(p);
      std::string modname(grpname);
      std::cout << modname << std::endl;

      std::string new_name;

      if (std::string::npos != modname.find("mat_"))
	{
	  new_name = "M_M"+std::to_string(nmat); 
	  nmat++;
	}
      else
	{
	  new_name = "M_VACUUM";
	}

      p = static_cast<const void*>(new_name.c_str());
      int length = NAME_TAG_SIZE;
      rval = MBI()->tag_set_by_ptr( name_tag, &grp, 1, &p, &length);
    }

  std::string out_file = "ouput.h5m";
  rval = MBI()->write_mesh(out_file.c_str(),&input_mesh_set,0);

  exit(1);

  // now have list of tags in the problem
  MBRange facets;
  rval = MBI()->get_entities_by_dimension(input_mesh_set, 2, facets);

  rval = MBI()->tag_get_handle(NAME_TAG_NAME, NAME_TAG_SIZE, MB_TYPE_OPAQUE, name_tag );


  //  rval = MBI()->tag_get_handle("NAME",32,MB_TYPE_OPAQUE,name_tag);

  std::vector<std::string> values;
  for(MBRange::iterator af_it = facets.begin() ; af_it != facets.end() ; ++af_it)
    {
      std::string tmp;
      rval = MBI()->tag_get_data(name_tag,&(*af_it),32,&tmp);
      values.push_back(tmp);
    }

  std::vector<std::string>::const_iterator values_it;
  for( values_it = values.begin() ; values_it != values.end() ; ++values_it )
    {
	std::cout << *values_it << std::endl;
    }


  exit(0);





  // loop over the mat_set
  MBTag mat_set_tag;
  rval = MBI()->tag_get_handle("MATERIAL_SET",1,MB_TYPE_INTEGER,mat_set_tag);
  std::vector<int> value;
  for(MBRange::iterator f_it = facets.begin() ; f_it != facets.end() ; ++f_it)
    {
      int tmp;
      rval = MBI()->tag_get_data(mat_set_tag,&(*f_it),1,&tmp);
      value.push_back(tmp);
    }

  int lval = 0;
  std::vector<int>::const_iterator tgs;
  for( tgs = value.begin() ; tgs != value.end() ; ++tgs )
    {
	std::cout << *tgs << std::endl;
    }

  exit(0);
  
  MBRange dagmc_mat;
  MBTag dagmc_tag;
  std::vector<std::string> dagmc_matnum;

  rval = MBI()->tag_get_handle("NAME",0,MB_TYPE_OPAQUE,
  			       dagmc_tag,moab::MB_TAG_DENSE);
  if(rval!=MB_SUCCESS)
    {
      std::cout << "Could not get the handle" << std::endl;
      exit(1);
    }
  rval = MBI()->get_entities_by_type_and_tag(input_mesh_set,MBENTITYSET
  			      ,&dagmc_tag,NULL,1,dagmc_mat, MBInterface::UNION);
  if(rval!=MB_SUCCESS)
    {
      std::cout << "Could not get the entities" << std::endl;
      exit(1);
    }
  rval = MBI()->tag_get_data(dagmc_tag,dagmc_mat,&dagmc_matnum);
    {
      std::cout << "Could not get the tag data" << std::endl;
      exit(1);
    }

    for( std::vector<std::string>::const_iterator i=dagmc_matnum.begin() ; i <= dagmc_matnum.end() ; i++)
    {
      std::cout << *i << std::endl;
    }

  std::cout << dagmc_mat << std::endl;
  std::cout << sizeof(dagmc_mat) << std::endl;

  return 0;
}

MBInterface *MBI() 
{
    static MBCore instance;
    return &instance;
}
