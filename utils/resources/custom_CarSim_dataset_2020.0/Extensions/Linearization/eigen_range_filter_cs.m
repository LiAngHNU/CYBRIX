function [eig_filtered]=eigen_range_filter(eig_all);

len=length(eig_all);

mm=0;
for nn = 1:len
    if real(eig_all(nn,1)) < -20
    elseif real(eig_all(nn,1)) > 5
    elseif imag(eig_all(nn,1)) < -2
    elseif imag(eig_all(nn,1)) > 40
    else
        mm=mm+1;
        eig_filtered(mm)=eig_all(nn,1);
    end
end