class AppList < ActiveRecord::Base
  attr_accessible :app_name_id, :port, :vm_list_id  
  belongs_to :vm_list  
  belongs_to :app_name    
end
